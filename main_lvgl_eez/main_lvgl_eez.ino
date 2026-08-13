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

#include "ui.h"
#include "screens.h"

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
#define TOUCH_MIN_X 3372
#define TOUCH_MAX_X 554
#define TOUCH_MIN_Y 3651
#define TOUCH_MAX_Y 345

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

void go_to_main_screen(lv_timer_t *t) {
  lv_scr_load_anim(objects.main, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);
  lv_timer_del(startup_timer);
  startup_timer = NULL;
}

void setup() {
  Serial.begin(115200);

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

  // --- Boot sequence: startup screen for 2s, then main screen ---
  lv_scr_load(objects.startup);
  startup_timer = lv_timer_create(go_to_main_screen, 2000, NULL);
  lv_timer_set_repeat_count(startup_timer, 1);
}

void loop() {
  lv_timer_handler();
  ui_tick();
  delay(5);
}
