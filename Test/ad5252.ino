#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <AD5252.h>

// =============================
// Device definitions
// =============================

#define DIGIPOT_ADDR 0x2C

Adafruit_ADS1115 ads;
AD5252 digipot(DIGIPOT_ADDR);

// =============================
// Setup
// =============================

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  Serial.println();
  Serial.println("=============================");
  Serial.println(" AD5252 + ADS1115 TEST");
  Serial.println("=============================");
  Serial.println();

  // -----------------------------
  // Initialize AD5252
  // -----------------------------

  if (digipot.begin())
  {
    Serial.println("AD5252 Connected!");
  }
  else
  {
    Serial.println("AD5252 NOT found!");
    while (1);
  }

  // Set initial RDAC value = 0
  digipot.write(0, 0);

  Serial.print("Initial RDAC value: ");
  Serial.println(digipot.read(0));

  // -----------------------------
  // Initialize ADS1115
  // -----------------------------

  if (!ads.begin(0x48))
  {
    Serial.println("ADS1115 not found!");
    while (1);
  }

  Serial.println("ADS1115 Connected!");

  // ±6.144 V range
  ads.setGain(GAIN_TWOTHIRDS);

  Serial.println("ADS1115 configured");
  Serial.println();

  Serial.println("Enter AD5252 digital value (0-255):");
  Serial.println();
}

// =============================
// Loop
// =============================

void loop()
{
  // =============================
  // Serial input for AD5252
  // =============================

  if (Serial.available() > 0)
  {
    int value = Serial.parseInt();

    if (value >= 0 && value <= 255)
    {
      // Write digital value to AD5252
      digipot.write(0, value);

      delay(10);

      // Read back value
      int readValue = digipot.read(0);

      Serial.println();
      Serial.println("-----------------------------");

      Serial.print("Written value : ");
      Serial.println(value);

      Serial.print("Readback value: ");
      Serial.println(readValue);

      Serial.println("-----------------------------");
      Serial.println();

      Serial.println("Enter next value (0-255):");
    }
    else
    {
      Serial.println();
      Serial.println("Invalid value!");
      Serial.println("Please enter a value from 0 to 255.");
      Serial.println();
    }

    // Clear remaining serial characters
    while (Serial.available())
    {
      Serial.read();
    }
  }

  // =============================
  // ADS1115 voltage measurement
  // =============================

  int16_t adcValue = ads.readADC_SingleEnded(0);

  float voltage = ads.computeVolts(adcValue);

  Serial.print("ADS1115 AIN0: ");
  Serial.print(adcValue);

  Serial.print("\tVoltage: ");
  Serial.print(voltage, 6);
  Serial.println(" V");

  delay(500);
}