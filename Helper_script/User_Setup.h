// ============================================================
// User_Setup.h  --  for TFT_eSPI library
// ESP32 + ILI9488 (3.5" SPI TFT with XPT2046 resistive touch)
// ============================================================
//
// HOW TO USE:
// 1. Find your TFT_eSPI library folder, usually at:
//      Windows: Documents\Arduino\libraries\TFT_eSPI
//      Mac:     ~/Documents/Arduino/libraries/TFT_eSPI
//      Linux:   ~/Arduino/libraries/TFT_eSPI
// 2. Rename/back up the existing User_Setup.h in that folder.
// 3. Copy THIS file in and rename it to "User_Setup.h".
// 4. Edit the pin numbers below (Pin Config section) to match
//    exactly how you wired your display to the ESP32.
//
// NOTE: Some TFT_eSPI versions instead want you to select a
// setup via User_Setup_Select.h. If yours uses that, just make
// sure only ONE setup file is active and it contains these
// settings.
// ============================================================

// ---------------- Driver ----------------
#define ILI9488_DRIVER

// ---------------- Display resolution ----------------
// ILI9488 panels are almost always 320x480
#define TFT_WIDTH  320
#define TFT_HEIGHT 480

// ---------------- Pin Config (EDIT THESE) ----------------
// Change these to match your actual ESP32 <-> display wiring.
// These are common defaults for many ESP32 dev boards.

// Display is on the default VSPI bus.
// Pins per user's schematic (U10 ILI9488 module):
#define TFT_MISO 19   // SDO(MISO)
#define TFT_MOSI 23   // SDI(MOSI)
#define TFT_SCLK 18   // SCK
#define TFT_CS   15   // CS
#define TFT_DC   32   // DC/RS
#define TFT_RST  27   // RESET

// NOTE: LED (backlight) is wired through a fixed 10R resistor straight to
// 3.3V on this board (see R2 in schematic) - always on, no GPIO control,
// so no TFT_BL / TFT_BACKLIGHT_ON define is needed.

// NOTE: Touch is NOT defined here on purpose.
// Touch (XPT2046) is wired to its own separate pins (GPIO 14/16/17/25/26),
// which don't match either standard VSPI or HSPI pin mapping, so it's
// initialized manually in the sketch via SPI.begin(sck, miso, mosi, cs)
// rather than through TFT_eSPI. Do NOT define TOUCH_CS here.

// ---------------- Fonts ----------------
// Comment out any fonts you don't need to save flash space
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

// ---------------- SPI frequency ----------------
#define SPI_FREQUENCY       40000000   // Display SPI speed
#define SPI_READ_FREQUENCY  20000000   // Display read speed
#define SPI_TOUCH_FREQUENCY  2500000   // Touch SPI speed (keep this low)

// ---------------- Misc ----------------
// If your ESP32 board uses non-default SPI pins/bus, you can
// also force VSPI/HSPI explicitly - usually not needed:
// #define USE_HSPI_PORT
