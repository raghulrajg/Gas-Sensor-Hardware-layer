# Gas Sensor Hardware Layer

Hardware layer for gas sensor data acquisition, real-time monitoring UI, and digital sensor calibration on ESP32. Provides sensor communication, a touch-driven LVGL graphical interface, and live measurement visualization.

## Overview

This project drives a 480x320 ILI9488 TFT display with a resistive XPT2046 touch panel on an ESP32, running a GUI designed in [EEZ Studio](https://www.envox.io/eez-studio/) and rendered with [LVGL](https://lvgl.io/) v9. The UI presents live readings from a bank of gas sensors, with dedicated screens for monitoring, configuration, and sensor calibration.

## Features

- Touch-driven LVGL 9.x GUI exported from EEZ Studio
- Startup splash screen with automatic transition to the main dashboard
- Live sensor data screen with real-time value visualization
- Dedicated sensor list / detail screens
- Settings and about screens
- Calibrated resistive touch input (XPT2046) on a separate SPI bus from the display
- Heap-allocated LVGL draw buffers, tuned to run within ESP32 internal RAM (no PSRAM required)

## Supported Sensors

| Sensor | Description |
|---|---|
| TGS825 | Hydrogen sulfide (H2S) gas sensor |
| TGS2802 | Odor / VOC gas sensor |
| TGS1820 | Ethanol / alcohol vapor sensor |
| MQ-3 | Alcohol gas sensor |
| MQ-130 | Formaldehyde / organic solvent vapor sensor |
| MQ-137 | Ammonia (NH3) gas sensor |
| WSP2110 | VOC / air quality sensor |

## Hardware

- ESP32 development board
- ILI9488 480x320 TFT display (VSPI bus)
- XPT2046 resistive touch controller (separate HSPI bus)
- Gas sensor array (see table above)

### Display Wiring (VSPI, via TFT_eSPI `User_Setup.h`)

Configured in `Helper_script/User_Setup.h` — MISO, MOSI, SCLK, CS, DC, and RST pins for the ILI9488 driver.

### Touch Wiring (separate HSPI bus)

| Signal | GPIO |
|---|---|
| T_CLK | 14 |
| T_CS | 26 |
| T_DIN | 17 |
| T_DO | 16 |
| T_IRQ | 25 |

Touch runs on its own SPI bus, separate from the display, since the ILI9488 does not reliably tri-state MISO for a shared bus.

### Touch Calibration

```
TOUCH_MIN_X = 3372   TOUCH_MAX_X = 554
TOUCH_MIN_Y = 3651   TOUCH_MAX_Y = 345
```

## Repository Structure

```
Gas-Sensor-Hardware-layer/
├── main_lvgl_eez/       # Main Arduino sketch: display/touch/LVGL bridge + EEZ Studio UI export
├── Helper_script/       # lv_conf.h and User_Setup.h reference configs for display/touch pin setup
├── Test/                # Test sketch that sends simulated sensor values to validate the UI
└── LICENSE              # GPL-3.0
```

## Getting Started

### Prerequisites

Install the following via Arduino Library Manager (unless noted):

- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) (Bodmer)
- [XPT2046_Touchscreen](https://github.com/PaulStoffregen/XPT2046_Touchscreen) (Paul Stoffregen)
- [lvgl](https://github.com/lvgl/lvgl) v9.x
- [eez-framework](https://github.com/eez-open/eez-framework) — download from GitHub and place in your Arduino `libraries` folder (provides the EEZ Studio flow/GUI runtime; not available via Library Manager)

### Setup

1. Copy `Helper_script/User_Setup.h` into your `TFT_eSPI` library folder, replacing the default, to configure the ILI9488 display pins.
2. Copy `Helper_script/lv_conf.h` to sit next to your `lvgl` library folder (i.e. `Arduino/libraries/lv_conf.h`, a sibling of the `lvgl` folder — not inside it).
3. Open `main_lvgl_eez/main_lvgl_eez.ino` in Arduino IDE. All supporting `.c`/`.h` files must sit flat in the same sketch folder (Arduino does not compile files in subfolders automatically).
4. Select your ESP32 board, compile, and upload.
5. Use `Test/` to send simulated sensor values over serial for verifying the UI without the physical sensor array connected.

## Boot Behavior

On power-up, the startup splash screen displays for 2 seconds before automatically transitioning to the main dashboard screen.

## License

Distributed under the GPL-3.0 License. See `LICENSE` for details.
