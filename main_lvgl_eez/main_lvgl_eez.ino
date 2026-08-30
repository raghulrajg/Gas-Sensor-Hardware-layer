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
#include <Adafruit_ADS1X15.h>
#include <AD5252.h>
#include <AD5144A.h>
#include <string.h>

#include "ui.h"
#include "screens.h"

// ---------------- Temperature / Humidity sensor (AHT10, I2C) ----------------
Adafruit_AHT10 aht;
bool aht_ok = false;
unsigned long last_sensor_read = 0;
const unsigned long SENSOR_READ_INTERVAL_MS = 1000; // read once per second
static float last_temperature_c = 0;
static float last_humidity_rh = 0;

// ---------------- ADC + digital potentiometer hardware ----------------
// Two ADS1115 16-bit ADCs (one channel per sensor's analog output) and two
// digital potentiometers (AD5144A = 4-channel/10k, AD5252 = 2-channel/50k)
// used to set each sensor's load-resistance calibration.
#define ADS1_ADDR 0x48
#define ADS2_ADDR 0x49
#define DIGIPOT_10K_ADDR 0x2B // AD5144A, 4 channels, 0-10k ohm
#define DIGIPOT_50K_ADDR 0x2C // AD5252,  2 channels, 0-50k ohm

Adafruit_ADS1115 ads1; // 0x48
Adafruit_ADS1115 ads2; // 0x49
AD5144A digipot_10k(DIGIPOT_10K_ADDR);
AD5252 digipot_50k(DIGIPOT_50K_ADDR);

enum digipot_type_t { DIGIPOT_NONE, DIGIPOT_AD5144A, DIGIPOT_AD5252 };

// One entry per sensor: its setting-screen slider/label (NULL for TGS1820,
// which is a fixed resistor with no calibration control), which digipot
// and channel controls its load resistance, and which ADS1115 + channel
// reads its analog output for the graph.
typedef struct {
  const char *name;       // matches the Sensor_ID flow variable's value
  lv_obj_t **slider;       // setting-screen slider (NULL = none)
  lv_obj_t **label;        // setting-screen label (NULL = none)
  int32_t pot_out_max;     // calibration range in ohms (0 = no digipot)
  digipot_type_t pot_type;
  void *pot;
  uint8_t pot_channel;
  Adafruit_ADS1115 *ads;
  uint8_t ads_channel;
  uint8_t gas_index;       // 1-based index used by the PC app's C:<index>:<value> protocol
} sensor_config_t;

// gas_index values match config.CHANNELS in the PC app exactly:
// 1=TGS825 2=TGS2602 3=MQ137 4=TGS1820 5=MQ138 6=WSP2110 7=MQ3
static sensor_config_t sensor_configs[] = {
  { "TGS825",  &objects.tgs_825,  &objects.tgs825_label,  50000, DIGIPOT_AD5252,  &digipot_50k, 1, &ads2, 0, 1 },
  { "TGS2602", &objects.tgs_2602, &objects.tgs2602_label, 10000, DIGIPOT_AD5144A, &digipot_10k, 3, &ads2, 3, 2 },
  { "TGS1820", NULL,              NULL,                   0,     DIGIPOT_NONE,    NULL,        0, &ads1, 3, 4 }, // fixed resistance, no calibration
  { "MQ137",   &objects.mq_137,   &objects.mq137_label,   10000, DIGIPOT_AD5144A, &digipot_10k, 1, &ads2, 2, 3 },
  { "MQ3",     &objects.mq_3,     &objects.mq3_label,     10000, DIGIPOT_AD5144A, &digipot_10k, 2, &ads1, 2, 7 },
  { "MQ138",   &objects.mq_138,   &objects.mq138_label,   50000, DIGIPOT_AD5252,  &digipot_50k, 0, &ads1, 0, 5 },
  { "WSP2110", &objects.wsp_2110, &objects.wsp2110_label, 10000, DIGIPOT_AD5144A, &digipot_10k, 0, &ads1, 1, 6 },
};

static sensor_config_t *find_sensor_config(const char *name) {
  if (!name) return NULL;
  for (size_t i = 0; i < sizeof(sensor_configs) / sizeof(sensor_configs[0]); i++) {
    if (strcmp(sensor_configs[i].name, name) == 0) return &sensor_configs[i];
  }
  return NULL;
}

static sensor_config_t *find_sensor_by_gas_index(int gas_index) {
  for (size_t i = 0; i < sizeof(sensor_configs) / sizeof(sensor_configs[0]); i++) {
    if (sensor_configs[i].gas_index == gas_index) return &sensor_configs[i];
  }
  return NULL;
}

// Writes a 0-255 wiper value to whichever digipot the config points to.
static void write_digipot(sensor_config_t *cfg, uint8_t raw) {
  if (!cfg || cfg->pot_type == DIGIPOT_NONE) return;
  if (cfg->pot_type == DIGIPOT_AD5144A) {
    ((AD5144A *)cfg->pot)->write(cfg->pot_channel, raw);
  } else if (cfg->pot_type == DIGIPOT_AD5252) {
    ((AD5252 *)cfg->pot)->write(cfg->pot_channel, raw);
  }
}

// ---------------- PC application link (connection state) ----------------
// Declared up here (ahead of the calibration callbacks below) so both the
// setting-screen sliders and the show_data shared slider can notify the PC
// the moment the physical touchscreen changes a value - see
// notify_pc_calibration(). The rest of the PC link (serial parsing, D:
// streaming) lives further down, near send_current_data_to_pc().
static bool pc_connected = false;
static unsigned long last_pc_heartbeat_ms = 0;
const unsigned long PC_TIMEOUT_MS = 3000;

// Pushes an unsolicited "C:<gas_index>:<ohms>\n" to the PC so it stays in
// sync when the value changed locally (on-device slider), not from a PC
// command. Only sent while a PC is actually connected. ohm_value is the
// actual resistance in ohms (not a 0-100 percentage).
static void notify_pc_calibration(sensor_config_t *cfg, int32_t ohm_value) {
  if (!pc_connected || !cfg) return;
  Serial.print("C:");
  Serial.print(cfg->gas_index);
  Serial.print(":");
  Serial.println(ohm_value);
}

// ---------------- Calibration: single source of truth for BOTH screens ----------------
// show_data has ONE shared slider (calibration_bar) for whichever sensor is
// active (tracked by the EEZ Flow variable Sensor_ID), and the setting
// screen has its own dedicated slider per sensor. Whichever one the user
// actually drags, this function updates the digipot AND both UI widgets
// (setting-screen slider/label, and - if this sensor is the one currently
// shown - show_data's slider/label too), so the two screens never drift
// out of sync with each other.
//
// Sliders use their sensor's actual ohm range as their LVGL range (set in
// setup(), and re-set on calibration_bar whenever the active sensor
// changes) instead of the default 0-100 - so slider values ARE ohms
// directly, with no lossy 0-100 round-trip losing precision.
#include "vars.h"
#include "structs.h"

static void apply_calibration(sensor_config_t *cfg, int32_t ohm_value) {
  if (!cfg || cfg->pot_type == DIGIPOT_NONE) return;
  if (ohm_value < 0) ohm_value = 0;
  if (ohm_value > cfg->pot_out_max) ohm_value = cfg->pot_out_max;

  // ohm_value maps to the digipot's wiper INVERTED (255->0): max
  // resistance selected == wiper value 0.
  uint8_t pot_raw = (uint8_t)lv_map(ohm_value, 0, cfg->pot_out_max, 255, 0);
  write_digipot(cfg, pot_raw);

  char buf[16];
  snprintf(buf, sizeof(buf), "%ld", (long)ohm_value);

  // Setting screen's own slider/label for this sensor.
  if (cfg->slider) {
    lv_slider_set_value(*(cfg->slider), ohm_value, LV_ANIM_OFF);
    lv_label_set_text(*(cfg->label), buf);
  }

  // show_data's shared slider/label, only if this sensor is the one
  // currently displayed there.
  const char *active_sensor_id = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_SENSOR_ID).getString();
  if (active_sensor_id && strcmp(active_sensor_id, cfg->name) == 0) {
    lv_slider_set_range(objects.calibration_bar, 0, cfg->pot_out_max);
    lv_slider_set_value(objects.calibration_bar, ohm_value, LV_ANIM_OFF);
    char buf2[24];
    snprintf(buf2, sizeof(buf2), "%ld ohm", (long)ohm_value);
    lv_label_set_text(objects.calibration_data, buf2);
  }
}

// Shows/hides calibration_bar + calibration_data for whichever sensor is
// active, and re-syncs both from that sensor's setting-screen slider - the
// actual source of truth - rather than reusing calibration_bar's previous
// position (which may still belong to a different sensor/range). Used at
// boot and whenever show_data becomes the active screen.
static void update_calibration_data() {
  const char *sensor_id = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_SENSOR_ID).getString();
  sensor_config_t *cfg = find_sensor_config(sensor_id);

  if (!cfg || cfg->pot_type == DIGIPOT_NONE) {
    // TGS1820 is a fixed resistor - no calibration control for it.
    lv_obj_add_flag(objects.calibration_bar, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.calibration_data, LV_OBJ_FLAG_HIDDEN);
    return;
  }

  lv_obj_clear_flag(objects.calibration_bar, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(objects.calibration_data, LV_OBJ_FLAG_HIDDEN);

  lv_slider_set_range(objects.calibration_bar, 0, cfg->pot_out_max);

  // Fetch the CURRENT value from this sensor's own setting-screen slider -
  // the authoritative per-sensor value - not calibration_bar's old
  // position.
  int32_t ohm_value = cfg->slider ? lv_slider_get_value(*(cfg->slider)) : 0;
  apply_calibration(cfg, ohm_value);
}

// Live update while dragging the show_data slider. Also pushes the new
// value to the PC (if connected), since this is a locally driven change.
static void calibration_bar_event_cb(lv_event_t *e) {
  const char *sensor_id = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_SENSOR_ID).getString();
  sensor_config_t *cfg = find_sensor_config(sensor_id);
  if (!cfg || cfg->pot_type == DIGIPOT_NONE) return;

  int32_t ohm_value = lv_slider_get_value(objects.calibration_bar); // native ohm range
  apply_calibration(cfg, ohm_value);
  notify_pc_calibration(cfg, ohm_value);
}

// Refresh the range/label whenever show_data becomes the active screen,
// since Sensor_ID may have just changed (a different sensor was tapped
// on sensor_list right before this screen loaded).
static void show_data_screen_loaded_cb(lv_event_t *e) {
  update_calibration_data();
}

// ---------------- Real-time graph (FIFO circular buffer) ----------------
// A fixed-size ring buffer holds the most recent GRAPH_BUFFER_SIZE samples.
// New samples overwrite the oldest slot as the write index wraps around -
// that wraparound is what makes it a FIFO circular buffer (oldest data
// falls off as new data comes in, fixed memory footprint, no shifting
// of existing elements). The LVGL chart is fed one new point per push via
// lv_chart_set_next_value(), which visually scrolls the chart left to
// match - the two stay in sync sample-for-sample. At one sample per
// second (see GRAPH_UPDATE_INTERVAL_MS) and 30 points, the chart always
// shows the last 30 seconds.
//
// Y-axis is a fixed 0-5V, stored as millivolts (0-5000) so LVGL's integer
// chart points still give 1mV of plotting resolution. The underlying
// ADS1115 reading keeps full float precision (see read_current_sensor_mv)
// and is also printed to Serial with 6 decimal places.
#define GRAPH_BUFFER_SIZE 30
static int16_t graph_buffer[GRAPH_BUFFER_SIZE];
static uint8_t graph_buffer_index = 0; // next write position, wraps at GRAPH_BUFFER_SIZE

static lv_chart_series_t *graph_series = NULL;
unsigned long last_graph_update = 0;
const unsigned long GRAPH_UPDATE_INTERVAL_MS = 1000; // one new point every 1s

static void graph_buffer_push(int16_t value) {
  graph_buffer[graph_buffer_index] = value;
  graph_buffer_index = (graph_buffer_index + 1) % GRAPH_BUFFER_SIZE; // circular wrap
}

// Reads the ADS1115 channel for whichever sensor Sensor_ID currently
// points to, returning millivolts (0-5000) clamped for chart plotting.
static int16_t read_current_sensor_mv() {
  const char *sensor_id = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_SENSOR_ID).getString();
  sensor_config_t *cfg = find_sensor_config(sensor_id);
  if (!cfg || !cfg->ads) return 0;

  int16_t adc_raw = cfg->ads->readADC_SingleEnded(cfg->ads_channel);
  float voltage = cfg->ads->computeVolts(adc_raw); // full float precision

  Serial.print(cfg->name);
  Serial.print(" voltage: ");
  Serial.println(voltage, 6); // 6 decimal digits

  int32_t mv = (int32_t)(voltage * 1000.0f);
  if (mv < 0) mv = 0;
  if (mv > 5000) mv = 5000;
  return (int16_t)mv;
}

static void setup_graph() {
  lv_chart_set_type(objects.graph, LV_CHART_TYPE_LINE);
  lv_chart_set_point_count(objects.graph, GRAPH_BUFFER_SIZE);
  lv_chart_set_update_mode(objects.graph, LV_CHART_UPDATE_MODE_SHIFT);
  lv_chart_set_range(objects.graph, LV_CHART_AXIS_PRIMARY_Y, 0, 5000); // 0-5V in mV
  graph_series = lv_chart_add_series(objects.graph, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);

  // Seed the buffer and chart with real initial readings so it isn't
  // empty at boot.
  for (int i = 0; i < GRAPH_BUFFER_SIZE; i++) {
    int16_t mv = read_current_sensor_mv();
    graph_buffer_push(mv);
    lv_chart_set_next_value(objects.graph, graph_series, mv);
  }
}

// Called periodically from loop(). Pushes one new sample into the FIFO
// buffer and onto the chart.
static void update_graph() {
  int16_t mv = read_current_sensor_mv();
  graph_buffer_push(mv);
  lv_chart_set_next_value(objects.graph, graph_series, mv);
}

// ---------------- Setting screen sliders -> value labels ----------------
// The setting screen has its own slider per sensor (except TGS1820, which
// has none - fixed resistance). Unlike show_data's single shared slider,
// these map directly 1:1 - each slider always represents the same sensor -
// and reuse the same sensor_configs[] table for range + digipot wiring.
// Live update while dragging a setting-screen slider. Uses the same shared
// apply_calibration() as the show_data slider, so both screens stay in
// sync no matter which one triggered the change.
static void sensor_slider_event_cb(lv_event_t *e) {
  sensor_config_t *cfg = (sensor_config_t *)lv_event_get_user_data(e);
  int32_t ohm_value = lv_slider_get_value(*(cfg->slider)); // native ohm range (set in setup())
  apply_calibration(cfg, ohm_value);
  notify_pc_calibration(cfg, ohm_value);
}

// ---------------- PC application link ----------------
// Protocol (matches https://github.com/raghulrajg/Gas-Sensor-Monitor):
//   Device -> PC : "D:g1,g2,g3,g4,g5,g6,g7,temp,humidity\n"
//                   7 gas readings (in gas_index 1-7 order) + temp + humidity
//   PC -> Device : "C:<gas_index>:<value 0-100>\n"  - set that sensor's calibration
//   PC -> Device : "R:SETTINGS\n"                    - device replies with 7x "C:i:v\n"
//   PC -> Device : "H\n"                             - heartbeat (see below)
//
// Their app has no baked-in "are you still there" signal (only writes when a
// slider moves), so there's no way to tell "PC connected but idle" apart
// from "PC gone" without one. This adds a lightweight heartbeat: the PC side
// needs a matching one-line addition (see the serial_worker.py patch) that
// sends "H\n" once a second. ANY recognized line from the PC (heartbeat,
// calibration command, or settings request) counts as "still connected" and
// resets the timeout - so an active user isn't dependent on the heartbeat
// alone. If nothing arrives for PC_TIMEOUT_MS, we mark it disconnected and
// stop streaming "D:" lines entirely (per your requirement to not waste
// cycles/UART time when nothing is listening).
// (pc_connected / last_pc_heartbeat_ms / PC_TIMEOUT_MS are declared earlier,
// alongside notify_pc_calibration(), so the slider callbacks can use them.)
static unsigned long last_data_stream_ms = 0;

static char serial_line_buf[64];
static uint8_t serial_line_len = 0;

// Applies a PC-driven calibration command to the matching sensor: writes
// the digipot, and keeps both the setting-screen slider and (if that
// sensor is the one currently shown) the show_data screen's shared slider
// in visual sync with the PC's value.
// gas_index maps to the sensor per config.py; ohm_value is the actual
// resistance in ohms the PC wants set (not a 0-100 percentage).
static void apply_pc_calibration(int gas_index, float ohm_value_f) {
  sensor_config_t *cfg = find_sensor_by_gas_index(gas_index);
  if (!cfg) return; // unknown index - ignore
  if (cfg->pot_type == DIGIPOT_NONE) return; // e.g. TGS1820 - nothing to apply

  apply_calibration(cfg, (int32_t)ohm_value_f); // updates digipot + both screens' sliders/labels

  // No notify_pc_calibration here - the PC already knows (it sent this
  // value), echoing it back would just be redundant traffic.
}

// Replies to "R:SETTINGS" with each sensor's current calibration in ohms,
// one "C:<gas_index>:<ohms>\n" line per sensor, in gas_index order.
static void send_settings_to_pc() {
  for (int idx = 1; idx <= 7; idx++) {
    sensor_config_t *cfg = find_sensor_by_gas_index(idx);
    if (!cfg) continue;
    int32_t ohm_value = cfg->slider ? lv_slider_get_value(*(cfg->slider)) : 0;
    Serial.print("C:");
    Serial.print(idx);
    Serial.print(":");
    Serial.println(ohm_value);
  }
}

// Parses one complete line received from the PC.
static void process_serial_line(const char *line) {
  // Any recognized traffic (heartbeat included) counts as "PC present".
  last_pc_heartbeat_ms = millis();
  pc_connected = true;

  if (strcmp(line, "H") == 0) {
    return; // heartbeat only
  }

  if (strncmp(line, "R:SETTINGS", 10) == 0) {
    send_settings_to_pc();
    return;
  }

  if (line[0] == 'C' && line[1] == ':') {
    int idx = 0;
    float value = 0;
    if (sscanf(line, "C:%d:%f", &idx, &value) == 2) {
      apply_pc_calibration(idx, value);
    }
    return;
  }

  // Unknown line - ignore.
}

// Non-blocking line reader: accumulates characters until '\n'/'\r', then
// hands the completed line to process_serial_line(). Called every loop().
static void handle_serial_commands() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    if (c == '\n' || c == '\r') {
      if (serial_line_len > 0) {
        serial_line_buf[serial_line_len] = '\0';
        process_serial_line(serial_line_buf);
        serial_line_len = 0;
      }
    } else if (serial_line_len < sizeof(serial_line_buf) - 1) {
      serial_line_buf[serial_line_len++] = c;
    }
    // else: line too long/malformed - drop the overflow silently
  }
}

// Reads all 7 gas sensors (not just whichever is on-screen) plus the last
// temp/humidity reading, and prints one "D:" line - only ever called while
// pc_connected is true.
static void send_current_data_to_pc() {
  Serial.print("D:");
  for (int idx = 1; idx <= 7; idx++) {
    sensor_config_t *cfg = find_sensor_by_gas_index(idx);
    float voltage = 0;
    if (cfg && cfg->ads) {
      int16_t adc_raw = cfg->ads->readADC_SingleEnded(cfg->ads_channel);
      voltage = cfg->ads->computeVolts(adc_raw);
    }
    // NOTE: this sends raw sensor voltage, not ppm - converting to ppm
    // needs each sensor's datasheet resistance-ratio curve, which isn't
    // available here. The PC app just plots whatever numbers arrive.
    Serial.print(voltage, 2);
    Serial.print(",");
  }
  Serial.print(last_temperature_c, 2);
  Serial.print(",");
  Serial.println(last_humidity_rh, 2);
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
#define TOUCH_MAX_X 500
#define TOUCH_MIN_Y 3900
#define TOUCH_MAX_Y 300

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
  lv_label_set_text(title, "Device(s) Not Found");
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
  lv_label_set_text(footer, "Please Contract Redoxtronics Support for Assistance");
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

  // --- ADC + digipot hardware init ---
  // Must happen before wiring any slider or calling setup_graph(), since
  // both read/write through these device objects.
  if (!ads1.begin(ADS1_ADDR)) {
    Serial.println("ADS1115 @0x48 not found - check wiring.");
  }
  if (!ads2.begin(ADS2_ADDR)) {
    Serial.println("ADS1115 @0x49 not found - check wiring.");
  }
  ads1.setGain(GAIN_TWOTHIRDS); // +/-6.144V range
  ads2.setGain(GAIN_TWOTHIRDS);
  if (!digipot_10k.begin()) {
    Serial.println("AD5144A (10k digipot) not found - check wiring.");
  }
  if (!digipot_50k.begin()) {
    Serial.println("AD5252 (50k digipot) not found - check wiring.");
  }

  // --- Wire up calibration_bar / calibration_data on the show_data screen ---
  lv_obj_add_event_cb(objects.calibration_bar, calibration_bar_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(objects.show_data, show_data_screen_loaded_cb, LV_EVENT_SCREEN_LOADED, NULL);
  update_calibration_data(); // populate the label immediately at boot
  setup_graph(); // configure the chart, series, and seed the FIFO buffer

  // --- Wire up the setting screen's sensor sliders to their labels ---
  // (skips TGS1820, whose slider field is NULL - fixed resistance)
  for (size_t i = 0; i < sizeof(sensor_configs) / sizeof(sensor_configs[0]); i++) {
    if (sensor_configs[i].slider == NULL) continue;
    // Give each slider its sensor's real ohm range instead of LVGL's
    // default 0-100, so its value IS ohms directly (see apply_calibration).
    lv_slider_set_range(*(sensor_configs[i].slider), 0, sensor_configs[i].pot_out_max);
    lv_obj_add_event_cb(*(sensor_configs[i].slider), sensor_slider_event_cb, LV_EVENT_VALUE_CHANGED, &sensor_configs[i]);
    lv_obj_send_event(*(sensor_configs[i].slider), LV_EVENT_VALUE_CHANGED, NULL);
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

  // --- Handle incoming PC commands (heartbeat, calibration, settings request) ---
  handle_serial_commands();

  // --- PC connection timeout: no recognized traffic for PC_TIMEOUT_MS means gone ---
  if (pc_connected && millis() - last_pc_heartbeat_ms > PC_TIMEOUT_MS) {
    pc_connected = false;
  }

  // --- Update temperature/humidity labels on the show_data screen ---
  if (aht_ok && millis() - last_sensor_read >= SENSOR_READ_INTERVAL_MS) {
    last_sensor_read = millis();

    sensors_event_t humidity_event, temp_event;
    aht.getEvent(&humidity_event, &temp_event);
    last_temperature_c = temp_event.temperature;
    last_humidity_rh = humidity_event.relative_humidity;

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

  // --- Stream all 7 sensors + temp/humidity to the PC, only while connected ---
  if (pc_connected && millis() - last_data_stream_ms >= GRAPH_UPDATE_INTERVAL_MS) {
    last_data_stream_ms = millis();
    send_current_data_to_pc();
  }

  delay(5);
}
