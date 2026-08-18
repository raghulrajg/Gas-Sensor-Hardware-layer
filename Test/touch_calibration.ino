#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
//----------------------------------------

// ---- Touch pins per schematic (U10 ILI9488 module) ----
// These do NOT match ESP32's default VSPI or HSPI pin mapping, so the
// SPI bus for touch is remapped manually with SPI.begin() below.
#define T_CLK_PIN   14
#define T_CS_PIN    26
#define T_DIN_PIN   17
#define T_DO_PIN    16
#define T_IRQ_PIN   25

// Defines the width and height of the screen, also the font size.
// ILI9488 is 320 x 480 (native portrait). Adjust if you use setRotation()
// elsewhere - these values should match tft.width()/tft.height() as used.
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480
#define FONT_SIZE 2

int centerX, centerY;

byte ts_Calibration_Sequence = 1;
byte cnt = 0;

uint16_t ts_Min_X, ts_Max_X, ts_Min_Y, ts_Max_Y;
bool FT = true;

// Declaring the "TFT_eSPI" object as "tft".
TFT_eSPI tft = TFT_eSPI();

// Touch uses its own SPI instance since its pins don't match the
// display's SPI bus.
SPIClass touchSPI(HSPI);
XPT2046_Touchscreen touchscreen(T_CS_PIN, T_IRQ_PIN);


void Screen_Display_Test() {
  Serial.println();
  Serial.println("Screen Display Test");

  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
  tft.drawCentreString("Screen Display Test", centerX, centerY, FONT_SIZE);
  delay(2000);

  for (byte i = 0; i < 4; i++) {
    tft.setRotation(i);  //--> Range from 0 to 3.

    tft.fillScreen(TFT_WHITE);

    tft.setCursor(10, 10);
    tft.setTextColor(TFT_BROWN); tft.setTextSize(1);
    tft.print("LCD TFT Touchscreen");
    tft.setCursor(9, 25);
    tft.print("ILI9488 320*480 Pixel");
    tft.setTextColor(TFT_GREEN); tft.setTextSize(2);
    tft.setCursor(10, 40);
    tft.print("with ESP32");
    tft.setCursor(10, 60);
    tft.print("& TFT_eSPI Library");
    tft.setTextColor(TFT_CYAN); tft.setTextSize(2);
    tft.setCursor(10, 85);
    tft.print("UTEH STR");

    tft.fillRect(10, 110, 20, 20, TFT_PURPLE);
    tft.fillCircle(45, 120, 10, TFT_OLIVE);
    tft.fillTriangle(70, 110, 60, 130, 80, 130, TFT_BLUE);

    delay(1000);
  }

  tft.setTextSize(1);
  tft.setRotation(0); // back to native portrait for calibration
}

void get_Touchscreen_Min_X() {
  print_Title("touchscreen_Min_X");

  while (true) {
    tft.fillCircle(0, centerY, 5, TFT_RED);

    if (touchscreen.touched()) {
      cnt++;
      TS_Point p = touchscreen.getPoint();
      print_Val(p.x, p.y, p.z);

      if (FT == true && cnt > 1) {
        ts_Min_X = p.x;
        FT = false;
      }
      if (p.x < ts_Min_X) ts_Min_X = p.x;

      if (cnt > 30) {
        cnt = 0;
        FT = true;
        ts_Calibration_Sequence++;
        break;
      }
      delay(50);
    }
  }

  print_Rslt("touchscreen_Min_X", ts_Min_X);
  delay(2000);
}

void get_Touchscreen_Max_X() {
  print_Title("touchscreen_Max_X");

  while (true) {
    tft.fillCircle(SCREEN_WIDTH - 1, centerY, 5, TFT_RED);

    if (touchscreen.touched()) {
      cnt++;
      TS_Point p = touchscreen.getPoint();
      print_Val(p.x, p.y, p.z);

      if (FT == true && cnt > 3) {
        ts_Max_X = p.x;
        FT = false;
      }
      if (p.x > ts_Max_X) ts_Max_X = p.x;

      if (cnt > 30) {
        cnt = 0;
        FT = true;
        ts_Calibration_Sequence++;
        break;
      }
      delay(50);
    }
  }

  print_Rslt("touchscreen_Max_X", ts_Max_X);
  delay(2000);
}

void get_Touchscreen_Min_Y() {
  print_Title("touchscreen_Min_Y");

  while (true) {
    tft.fillCircle(centerX, 0, 5, TFT_RED);

    if (touchscreen.touched()) {
      cnt++;
      TS_Point p = touchscreen.getPoint();
      print_Val(p.x, p.y, p.z);

      if (FT == true && cnt > 3) {
        ts_Min_Y = p.y;
        FT = false;
      }
      if (p.y < ts_Min_Y) ts_Min_Y = p.y;

      if (cnt > 30) {
        cnt = 0;
        FT = true;
        ts_Calibration_Sequence++;
        break;
      }
      delay(50);
    }
  }

  print_Rslt("touchscreen_Min_Y", ts_Min_Y);
  delay(2000);
}

void get_Touchscreen_Max_Y() {
  print_Title("touchscreen_Max_Y");

  while (true) {
    tft.fillCircle(centerX, SCREEN_HEIGHT - 1, 5, TFT_RED);

    if (touchscreen.touched()) {
      cnt++;
      TS_Point p = touchscreen.getPoint();
      print_Val(p.x, p.y, p.z);

      if (FT == true && cnt > 3) {
        ts_Max_Y = p.y;
        FT = false;
      }
      if (p.y > ts_Max_Y) ts_Max_Y = p.y;

      if (cnt > 30) {
        cnt = 0;
        FT = true;
        ts_Calibration_Sequence++;
        break;
      }
      delay(50);
    }
  }

  print_Rslt("touchscreen_Max_Y", ts_Max_Y);
  delay(2000);
}

void print_Title(String ttl) {
  Serial.println();
  Serial.print("-----------------Get ");
  Serial.print(ttl);
  Serial.println(" value");
  Serial.println("Please touch inside the red area.");
  Serial.println();

  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawCentreString("Get " + ttl, centerX, 20, FONT_SIZE);
  tft.drawCentreString("value", centerX, 40, FONT_SIZE);
  tft.drawCentreString("Please touch inside", centerX, (centerY - 8) - 10, FONT_SIZE);
  tft.drawCentreString("the red area.", centerX, (centerY - 8) + 10, FONT_SIZE);
}

void print_Val(uint16_t x, uint16_t y, uint16_t z) {
  Serial.print("p.x = ");
  Serial.print(x);
  Serial.print(" | p.y = ");
  Serial.print(y);
  Serial.print(" | Pressure = ");
  Serial.println(z);

  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);

  int textY = 80;

  String rslt_Text = "p.x = " + String(x);
  tft.drawCentreString(rslt_Text, centerX, textY, FONT_SIZE);

  textY += 20;
  rslt_Text = "p.y = " + String(y);
  tft.drawCentreString(rslt_Text, centerX, textY, FONT_SIZE);

  textY += 20;
  rslt_Text = "Pressure = " + String(z);
  tft.drawCentreString(rslt_Text, centerX, textY, FONT_SIZE);
}

void print_Rslt(String label, uint16_t val) {
  Serial.print(label);
  Serial.print(" = ");
  Serial.println(val);

  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_DARKGREEN, TFT_WHITE);
  tft.drawCentreString(label, centerX, centerY - 10, FONT_SIZE);
  tft.drawCentreString(String(val), centerX, centerY + 10, FONT_SIZE);
}

void print_Final_Result() {
  Serial.println();
  Serial.println("=== Calibration Complete ===");
  Serial.println("Copy these values into your drawing board sketch:");
  Serial.println();
  Serial.print("#define TOUCH_MIN_X "); Serial.println(ts_Min_X);
  Serial.print("#define TOUCH_MAX_X "); Serial.println(ts_Max_X);
  Serial.print("#define TOUCH_MIN_Y "); Serial.println(ts_Min_Y);
  Serial.print("#define TOUCH_MAX_Y "); Serial.println(ts_Max_Y);
  Serial.println();

  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_DARKGREEN, TFT_WHITE);
  tft.drawCentreString("Calibration Done!", centerX, 20, FONT_SIZE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawCentreString("Check Serial Monitor", centerX, 40, FONT_SIZE);

  int y = 80;
  tft.drawCentreString("Min X: " + String(ts_Min_X), centerX, y, FONT_SIZE); y += 20;
  tft.drawCentreString("Max X: " + String(ts_Max_X), centerX, y, FONT_SIZE); y += 20;
  tft.drawCentreString("Min Y: " + String(ts_Min_Y), centerX, y, FONT_SIZE); y += 20;
  tft.drawCentreString("Max Y: " + String(ts_Max_Y), centerX, y, FONT_SIZE);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("=== Touch Calibration Starting ===");

  // --- Display init (VSPI, via TFT_eSPI / User_Setup.h) ---
  tft.init();
  tft.setRotation(0); // native portrait, matches SCREEN_WIDTH/HEIGHT above

  centerX = SCREEN_WIDTH / 2;
  centerY = SCREEN_HEIGHT / 2;

  // --- Touch init (separate SPI bus, remapped to schematic pins) ---
  touchSPI.begin(T_CLK_PIN, T_DO_PIN, T_DIN_PIN, T_CS_PIN);
  touchscreen.begin(touchSPI);
  touchscreen.setRotation(0); // match tft.setRotation()

  Screen_Display_Test();

  ts_Min_X = 0;
  ts_Max_X = 0;
  ts_Min_Y = 0;
  ts_Max_Y = 0;
}

void loop() {
  switch (ts_Calibration_Sequence) {
    case 1:
      get_Touchscreen_Min_X();
      break;
    case 2:
      get_Touchscreen_Max_X();
      break;
    case 3:
      get_Touchscreen_Min_Y();
      break;
    case 4:
      get_Touchscreen_Max_Y();
      break;
    case 5:
      print_Final_Result();
      ts_Calibration_Sequence++; // move past 5 so this only runs once
      break;
    default:
      // Calibration finished - idle here.
      // Reset the ESP32 to run calibration again.
      delay(1000);
      break;
  }
}

