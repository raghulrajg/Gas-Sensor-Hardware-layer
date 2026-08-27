#include <eez-framework.h>

/*
  ESP32 + ILI9488 + LVGL 9.x + EEZ Studio UI
  --------------------------------------------
  Glue sketch that connects your existing display/touch wiring
  (from your TFT_eSPI drawing-board example) to the LVGL GUI you
  exported from EEZ Studio (ui/screens.c, ui.c, etc).

  WHAT THIS FILE DOES:
    1. Inits the ILI9488 display on the default VSPI bus (via TFT_eSPI,
       pins set in User_Setup.h, same as your example).
    2. Inits the XPT2046 touch controller on a SEPARATE HSPI bus
       (same reasoning as your example: ILI9488 doesn't tri-state
       MISO properly, so touch needs its own bus).
    3. Bridges both into LVGL 9.x as a display driver + input device.
    4. Calls ui_init() from the EEZ-generated ui.c to build all screens.
    5. Shows the STARTUP screen first, waits 2 seconds, then switches
       to the MAIN screen.

  YOUR CALIBRATION VALUES (carried over unchanged from your example):
    TOUCH_MIN_X = 3372   TOUCH_MAX_X = 554
    TOUCH_MIN_Y = 3651   TOUCH_MAX_Y = 345

  FOLDER LAYOUT REQUIRED:
    <sketch_folder>/main_lvgl_eez.ino   <- this file
    <sketch_folder>/screens.c / screens.h / ui.c / ui.h / images.c /
      images.h / styles.c / styles.h / fonts.h / actions.h / vars.h /
      structs.h / ui_image_*.c   <- all your EEZ Studio export files,
        FLAT in the same folder as this .ino (NOT in a subfolder).
        Arduino IDE only auto-compiles .c/.cpp files that sit directly
        next to the .ino - it does NOT recurse into subfolders. If
        these files are inside a "ui/" subfolder, ui.c and screens.c
        never get compiled and you'll get "undefined reference to
        ui_init" / "undefined reference to ui_tick" linker errors.

  LIBRARIES REQUIRED (Library Manager):
    - TFT_eSPI              (Bodmer)
    - XPT2046_Touchscreen   (Paul Stoffregen)
    - lvgl                  (v9.x  -- must match what EEZ Studio exported)
    - eez-flow-lvgl / eez-framework   <- the runtime library EEZ Studio's
      LVGL 9 export depends on (provides eez/flow/lvgl_api.h,
      eez_flow_init, eez_flow_tick, getFlowState, etc). Install this from
      EEZ Studio's own "Export as Arduino/PlatformIO project" if you
      don't already have it - it is NOT optional, ui.c/screens.c will
      not compile without it.

  TFT_eSPI User_Setup.h (unchanged from your example):
      Driver: ILI9488_DRIVER
      Display pins on VSPI (MISO 19, MOSI 23, SCLK 18, CS 15, DC 2, RST 4)
      NO touch pins in User_Setup.h - touch is handled separately below.

  WIRING (touch side, separate SPI bus):
      T_CLK -> GPIO 14
      T_CS  -> GPIO 26
      T_DIN -> GPIO 17
      T_DO  -> GPIO 16
      T_IRQ -> GPIO 25
*/

// These two defines must come before any LVGL / eez-framework include.
// EEZ_FOR_LVGL: without it, eez-framework's ActionExecFunc type isn't
//   defined and ui.c fails with "unknown type name 'ActionExecFunc'".
// LV_LVGL_H_INCLUDE_SIMPLE: eez-framework's internal lvgl_api.h does
//   `#include "lvgl/lvgl.h"` by default; this makes it use `#include
//   "lvgl.h"` instead, matching how the Arduino lvgl library is laid out.
#define EEZ_FOR_LVGL
#define LV_LVGL_H_INCLUDE_SIMPLE

#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <lvgl.h>
#include <Wire.h>
#include <Adafruit_AHT10.h>
#include <string.h>

#include "ui.h"
#include "screens.h"

// ---------------- Temperature / Humidity sensor (AHT10, I2C) ----------------
Adafruit_AHT10 aht;
bool aht_ok = false;
unsigned long last_sensor_read = 0;
const unsigned long SENSOR_READ_INTERVAL_MS = 1000; // read once per second

// ---------------- calibration_bar / calibration_data (show_data screen) ----------------
// show_data has ONE slider (calibration_bar) and ONE value label
// (calibration_data), reused for whichever sensor was tapped on the
// sensor_list screen. Which sensor that was is tracked by the EEZ Flow
// global variable Sensor_ID (a text value, e.g. "TGS825") - the flow
// already sets this when navigating here, and it's what drives the
// sensor name shown at the top of this screen. We read that same
// variable to pick the right 0-max range for the slider.
#include "vars.h"
#include "structs.h"

static int32_t sensor_id_out_max(const char *sensor_id) {
  if (!sensor_id) return 100;
  if (strcmp(sensor_id, "TGS825")  == 0) return 50000;
  if (strcmp(sensor_id, "TGS2802") == 0) return 10000;
  if (strcmp(sensor_id, "TGS1820") == 0) return 50000;
  if (strcmp(sensor_id, "MQ137")   == 0) return 10000;
  if (strcmp(sensor_id, "MQ3")     == 0) return 10000;
  if (strcmp(sensor_id, "MQ130")   == 0) return 50;
  if (strcmp(sensor_id, "WSP2110") == 0) return 10;
  return 100; // fallback if Sensor_ID doesn't match a known sensor
}

static void update_calibration_data() {
  const char *sensor_id = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_SENSOR_ID).getString();
  int32_t out_max = sensor_id_out_max(sensor_id);

  int32_t raw = lv_slider_get_value(objects.calibration_bar); // 0-100 (LVGL default slider range)
  int32_t mapped = lv_map(raw, 0, 100, 0, out_max);

  char buf[16];
  snprintf(buf, sizeof(buf), "%ld", (long)mapped);
  lv_label_set_text(objects.calibration_data, buf);
}

// Live update while dragging the slider.
static void calibration_bar_event_cb(lv_event_t *e) {
  update_calibration_data();
}

// Refresh the range/label whenever show_data becomes the active screen,
// since Sensor_ID may have just changed (a different sensor was tapped
// on sensor_list right before this screen loaded).
static void show_data_screen_loaded_cb(lv_event_t *e) {
  update_calibration_data();
  configure_graph_range();
}

// ---------------- Real-time graph (FIFO circular buffer) ----------------
// A fixed-size ring buffer holds the most recent GRAPH_BUFFER_SIZE samples.
// New samples overwrite the oldest slot as the write index wraps around -
// that wraparound is what makes it a FIFO circular buffer (oldest data
// falls off as new data comes in, fixed memory footprint, no shifting
// of existing elements). The LVGL chart is fed one new point per push via
// lv_chart_set_next_value(), which visually scrolls the chart left to
// match - the two stay in sync sample-for-sample.
#define GRAPH_BUFFER_SIZE 30
static int16_t graph_buffer[GRAPH_BUFFER_SIZE];
static uint8_t graph_buffer_index = 0; // next write position, wraps at GRAPH_BUFFER_SIZE

static lv_chart_series_t *graph_series = NULL;
static int32_t graph_out_max = 100; // Y-axis max, updated per-sensor below
unsigned long last_graph_update = 0;
const unsigned long GRAPH_UPDATE_INTERVAL_MS = 1000; // one new point every 1s

static void graph_buffer_push(int16_t value) {
  graph_buffer[graph_buffer_index] = value;
  graph_buffer_index = (graph_buffer_index + 1) % GRAPH_BUFFER_SIZE; // circular wrap
}

// Re-ranges the chart's Y-axis to match whichever sensor Sensor_ID
// currently points to, reusing the same lookup the slider uses.
static void configure_graph_range() {
  const char *sensor_id = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_SENSOR_ID).getString();
  graph_out_max = sensor_id_out_max(sensor_id);
  lv_chart_set_range(objects.graph, LV_CHART_AXIS_PRIMARY_Y, 0, graph_out_max);
}

static void setup_graph() {
  lv_chart_set_type(objects.graph, LV_CHART_TYPE_LINE);
  lv_chart_set_point_count(objects.graph, GRAPH_BUFFER_SIZE);
  lv_chart_set_update_mode(objects.graph, LV_CHART_UPDATE_MODE_SHIFT);
  graph_series = lv_chart_add_series(objects.graph, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);

  configure_graph_range();

  // Seed the buffer and chart with initial points so it isn't empty at boot.
  for (int i = 0; i < GRAPH_BUFFER_SIZE; i++) {
    int16_t v = random(0, graph_out_max + 1); // TODO: replace with a real sensor reading
    graph_buffer_push(v);
    lv_chart_set_next_value(objects.graph, graph_series, v);
  }
}

// Called periodically from loop(). Pushes one new sample into the FIFO
// buffer and onto the chart.
static void update_graph() {
  int16_t value = random(0, graph_out_max + 1); // TODO: replace with a real sensor reading
  graph_buffer_push(value);
  lv_chart_set_next_value(objects.graph, graph_series, value);
}

// ---------------- Setting screen sliders -> value labels ----------------
// The setting screen has its own 7 sliders (one per sensor), each with its
// own adjacent label. Unlike show_data's single shared slider, these map
// directly 1:1 - each slider always represents the same sensor.
typedef struct {
  lv_obj_t **slider;
  lv_obj_t **label;
  int32_t out_max;
} sensor_slider_t;

static sensor_slider_t sensor_sliders[] = {
  { &objects.tgs_825,  &objects.tgs825_label,  50000 }, // TGS825:  0 - 50k
  { &objects.tgs_2802, &objects.tgs2802_label, 10000 }, // TGS2802: 0 - 10k
  { &objects.tgs_1820, &objects.tgs1820_label, 50000 }, // TGS1820: 0 - 50k
  { &objects.mq_137,   &objects.mq137_label,   10000 }, // MQ137:   0 - 10k
  { &objects.mq_3,     &objects.mq3_label,     10000 }, // MQ3:     0 - 10k
  { &objects.mq_130,   &objects.mq130_label,   50 },    // MQ130:   0 - 50
  { &objects.wsp_2110, &objects.wsp2110_label, 10 },    // WSP2110: 0 - 10
};

static void sensor_slider_event_cb(lv_event_t *e) {
  sensor_slider_t *cfg = (sensor_slider_t *)lv_event_get_user_data(e);
  int32_t raw = lv_slider_get_value(*(cfg->slider)); // 0-100 (LVGL default slider range)
  int32_t mapped = lv_map(raw, 0, 100, 0, cfg->out_max);
  char buf[16];
  snprintf(buf, sizeof(buf), "%ld", (long)mapped);
  lv_label_set_text(*(cfg->label), buf);
}

// ---------------- Display ----------------
TFT_eSPI tft = TFT_eSPI();

#define SCREEN_W 480
#define SCREEN_H 320

// ---------------- Touch (separate HSPI bus, per your schematic) ----------------
#define TOUCH_CLK 14
#define TOUCH_CS  26
#define TOUCH_DIN 17
#define TOUCH_DO  16
#define TOUCH_IRQ 25

SPIClass touchSPI(HSPI);
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

// ---------------- Touch calibration (from your example, unchanged) ----------------
#define TOUCH_MIN_X 3600
#define TOUCH_MAX_X 442
#define TOUCH_MIN_Y 3900
#define TOUCH_MAX_Y 271

// ---------------- LVGL draw buffer ----------------
// IMPORTANT: These are heap-allocated (not static arrays) because static
// buffers live in a small fixed-size DRAM region (.bss) that overflows
// easily on ESP32 boards without PSRAM. Heap memory comes from a much
// larger pool. Using 16 lines (not the full screen) keeps RAM usage low
// while still giving LVGL enough to render smoothly.
#define LVGL_BUF_LINES 16
static lv_color_t *buf1;
static lv_color_t *buf2;

static lv_display_t *disp;
static lv_indev_t *indev;

// ---------------- LVGL display flush callback ----------------
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)px_map, w * h, true);
  tft.endWrite();

  lv_display_flush_ready(disp);
}

// ---------------- LVGL touch read callback ----------------
// Same mapping logic as your example's getScreenTouch(), just adapted
// to LVGL's indev data struct instead of x/y references.
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  if (touch.touched()) {
    TS_Point p = touch.getPoint();

    int16_t x = map(p.x, TOUCH_MIN_X, TOUCH_MAX_X, 0, SCREEN_W);
    int16_t y = map(p.y, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, SCREEN_H);

    x = constrain(x, 0, SCREEN_W - 1);
    y = constrain(y, 0, SCREEN_H - 1);

    data->point.x = x;
    data->point.y = y;
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// ---------------- Startup -> Main screen transition ----------------
static lv_timer_t *startup_timer;

// ---------------- Required I2C devices ----------------
// Checked once, right when the startup delay expires. If any are missing,
// the boot halts on the startup screen with a warning instead of
// continuing to the main screen.
typedef struct {
  uint8_t addr;
  const char *name;
} i2c_device_t;

static const i2c_device_t required_i2c_devices[] = {
  { 0x38, "AHT10" },
  { 0x2C, "AD5252BRUZ50" },
  { 0x2B, "AD5144ABRUZ10" },
  { 0x49, "ADS1115IDGS" },
  { 0x48, "ADS1115IDGS" },
};

static lv_obj_t *i2c_warning_panel = NULL;

// Scans the bus for each required address. Returns true only if every
// device acknowledged. Missing devices (name + address) are written into
// missing_buf as a newline-separated list for display.
static bool check_required_i2c_devices(char *missing_buf, size_t missing_buf_size) {
  missing_buf[0] = '\0';
  bool all_found = true;

  for (size_t i = 0; i < sizeof(required_i2c_devices) / sizeof(required_i2c_devices[0]); i++) {
    Wire.beginTransmission(required_i2c_devices[i].addr);
    uint8_t result = Wire.endTransmission();

    if (result != 0) {
      all_found = false;
      char line[48];
      snprintf(line, sizeof(line), "0x%02X  %s\n", required_i2c_devices[i].addr, required_i2c_devices[i].name);
      strncat(missing_buf, line, missing_buf_size - strlen(missing_buf) - 1);
      Serial.print("I2C device not found: ");
      Serial.println(line);
    }
  }

  return all_found;
}

// Draws a red warning panel on top of the startup screen listing which
// devices didn't respond. Only ever created once per boot.
static void show_i2c_warning(const char *missing_list) {
  if (i2c_warning_panel) return;

  i2c_warning_panel = lv_obj_create(objects.startup);
  lv_obj_set_size(i2c_warning_panel, 420, 240);
  lv_obj_center(i2c_warning_panel);
  lv_obj_set_style_bg_color(i2c_warning_panel, lv_color_hex(0xB00020), 0);
  lv_obj_set_style_bg_opa(i2c_warning_panel, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(i2c_warning_panel, 8, 0);
  lv_obj_set_style_border_width(i2c_warning_panel, 0, 0);
  lv_obj_set_style_pad_all(i2c_warning_panel, 12, 0);
  lv_obj_clear_flag(i2c_warning_panel, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *title = lv_label_create(i2c_warning_panel);
  lv_label_set_text(title, "I2C Device(s) Not Found");
  lv_obj_set_style_text_color(title, lv_color_white(), 0);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 0);

  lv_obj_t *body = lv_label_create(i2c_warning_panel);
  lv_label_set_text(body, missing_list);
  lv_obj_set_style_text_color(body, lv_color_white(), 0);
  lv_obj_set_style_text_font(body, &lv_font_montserrat_14, 0);
  lv_label_set_long_mode(body, LV_LABEL_LONG_WRAP);
  lv_obj_set_width(body, 380);
  lv_obj_align(body, LV_ALIGN_CENTER, 0, 10);

  lv_obj_t *footer = lv_label_create(i2c_warning_panel);
  lv_label_set_text(footer, "Check wiring and power-cycle the device");
  lv_obj_set_style_text_color(footer, lv_color_white(), 0);
  lv_obj_set_style_text_font(footer, &lv_font_montserrat_14, 0);
  lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, 0);
}

void go_to_main_screen(lv_timer_t *t) {
  lv_timer_del(startup_timer);
  startup_timer = NULL;

  char missing_list[200];
  bool all_found = check_required_i2c_devices(missing_list, sizeof(missing_list));

  if (!all_found) {
    show_i2c_warning(missing_list);
    return; // halt here - do not proceed to the main screen
  }

  lv_scr_load_anim(objects.main, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
}

void setup() {
  Serial.begin(115200);

  // I2C bus for AHT10 and the other required chips (checked below, and
  // used by aht.begin() later) - init early so it's ready before the
  // startup-screen timer fires.
  Wire.begin();

  // --- Display on default VSPI bus ---
  tft.init();
  tft.setRotation(1); // landscape 480x320, matches EEZ project screen size

  // --- Touch on separate HSPI bus ---
  touchSPI.begin(TOUCH_CLK, TOUCH_DO, TOUCH_DIN, TOUCH_CS);
  touch.begin(touchSPI);
  touch.setRotation(1); // match tft.setRotation() above

  // --- LVGL init ---
  lv_init();
  lv_tick_set_cb(millis); // feeds LVGL's internal tick from millis()

  // Allocate draw buffers on the heap (see comment above buf1/buf2 decl).
  size_t buf_pixels = SCREEN_W * LVGL_BUF_LINES;
  buf1 = (lv_color_t *)malloc(buf_pixels * sizeof(lv_color_t));
  buf2 = (lv_color_t *)malloc(buf_pixels * sizeof(lv_color_t));
  if (!buf1 || !buf2) {
    Serial.println("FATAL: failed to allocate LVGL draw buffers - out of heap RAM");
    while (1) delay(1000);
  }

  disp = lv_display_create(SCREEN_W, SCREEN_H);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, buf1, buf2, buf_pixels * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);

  indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  // --- Build EEZ Studio UI (all screens: main/setting/show_data/about/
  //     sensor_list/startup) ---
  ui_init();

  // --- Wire up calibration_bar / calibration_data on the show_data screen ---
  lv_obj_add_event_cb(objects.calibration_bar, calibration_bar_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(objects.show_data, show_data_screen_loaded_cb, LV_EVENT_SCREEN_LOADED, NULL);
  update_calibration_data(); // populate the label immediately at boot
  setup_graph(); // configure the chart, series, and seed the FIFO buffer

  // --- Wire up the setting screen's 7 sensor sliders to their labels ---
  for (size_t i = 0; i < sizeof(sensor_sliders) / sizeof(sensor_sliders[0]); i++) {
    lv_obj_add_event_cb(*(sensor_sliders[i].slider), sensor_slider_event_cb, LV_EVENT_VALUE_CHANGED, &sensor_sliders[i]);
    lv_obj_send_event(*(sensor_sliders[i].slider), LV_EVENT_VALUE_CHANGED, NULL);
  }

  // --- Temp/humidity sensor init ---
  // AHT10 is I2C (Wire), so it doesn't conflict with the SPI buses used by
  // the display and touch above.
  aht_ok = aht.begin();
  if (!aht_ok) {
    Serial.println("AHT10 not found - check wiring. Continuing without sensor readings.");
  }

  // --- Boot sequence: startup screen for 2s, then main screen ---
  lv_scr_load(objects.startup);
  startup_timer = lv_timer_create(go_to_main_screen, 2000, NULL);
  lv_timer_set_repeat_count(startup_timer, 1);
}

void loop() {
  lv_timer_handler();
  ui_tick();

  // --- Update temperature/humidity labels on the show_data screen ---
  if (aht_ok && millis() - last_sensor_read >= SENSOR_READ_INTERVAL_MS) {
    last_sensor_read = millis();

    sensors_event_t humidity_event, temp_event;
    aht.getEvent(&humidity_event, &temp_event);

    char temp_buf[16];
    char hum_buf[16];
    snprintf(temp_buf, sizeof(temp_buf), "%.1f C", temp_event.temperature);
    snprintf(hum_buf, sizeof(hum_buf), "%.1f RH", humidity_event.relative_humidity);

    lv_label_set_text(objects.temperature, temp_buf);
    lv_label_set_text(objects.humidity, hum_buf);
  }

  // --- Push a new point into the real-time graph ---
  if (millis() - last_graph_update >= GRAPH_UPDATE_INTERVAL_MS) {
    last_graph_update = millis();
    update_graph();
  }

  delay(5);
}
