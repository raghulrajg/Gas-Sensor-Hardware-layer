/*
  gas_monitor_arduino_simulator.ino
  ----------------------------------
  Stands in for the real gas-sensor hardware so the Gas Sensor Monitor
  desktop app (main.py / serial_worker.py) can be tested end-to-end over
  an actual serial port, with every feature exercised:

    - streams simulated 7-gas + temperature + humidity readings
    - accepts a new calibration value any time a slider changes in the app
      and echoes back a confirmation line for it
    - answers "R:SETTINGS" (the app's File > Request Settings from Device)
      by reporting all 7 current calibration values

  WIRE PROTOCOL (must match config.py exactly)
  ----------------------------------------------
  Device -> App, one line per sample, 9 comma separated floats:
      D:<gas1>,<gas2>,<gas3>,<gas4>,<gas5>,<gas6>,<gas7>,<temp>,<humidity>\n
    e.g. D:23.10,41.52,8.03,0.00,55.21,12.44,90.02,24.50,55.20

  App -> Device, sent whenever a calibration slider/spinbox changes
  (index is 1-based, matches gas_index in config.CHANNELS):
      C:<index>:<value>\n
    e.g. C:3:25.50

  Device -> App, calibration confirmation (same format as above) - sent
  immediately after applying a C: command, AND once per gas sensor in
  reply to a settings request:
      C:<index>:<value>\n

  App -> Device, request current settings:
      R:SETTINGS\n

  BAUD RATE
  ---------
  config.BAUDRATE is fixed at 921600. That's reliable on boards with a
  native USB serial interface (ESP32, Arduino Due/Uno R4/Leonardo/Micro,
  Teensy, etc). If you're using a classic Uno/Nano (ATmega328P +
  FTDI/CH340 bridge), either use one of those boards instead, or lower
  BAUDRATE in config.py to something like 115200 and re-flash this
  sketch with the matching value below.
*/

const long BAUD_RATE = 921600;   // must match config.BAUDRATE in the app

const uint8_t NUM_GAS = 7;
const unsigned long SAMPLE_INTERVAL_MS = 200;   // ~5 Hz data stream

// Current calibration value per gas sensor (index 0..6 == gas_index 1..7).
// Simulated as a simple offset added to that channel's reading, so moving
// a slider in the app visibly shifts the corresponding curve - handy for
// confirming the calibration round-trip is actually working.
float calibration[NUM_GAS] = {0, 0, 0, 0, 0, 0, 0};

unsigned long lastSampleAt = 0;
String inputBuffer = "";

void setup() {
  Serial.begin(BAUD_RATE);
  while (!Serial) {
    ; // wait for native-USB boards to enumerate
  }
  randomSeed(analogRead(A0));
}

void loop() {
  readSerialCommands();

  unsigned long now = millis();
  if (now - lastSampleAt >= SAMPLE_INTERVAL_MS) {
    lastSampleAt = now;
    sendDataLine();
  }
}

// ---------------------------------------------------------------------
// Incoming command handling
// ---------------------------------------------------------------------
void readSerialCommands() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n') {
      handleLine(inputBuffer);
      inputBuffer = "";
    } else if (c != '\r') {
      inputBuffer += c;
      if (inputBuffer.length() > 64) {
        inputBuffer = "";  // guard against a runaway/garbled line
      }
    }
  }
}

void handleLine(String line) {
  line.trim();
  if (line.length() == 0) return;

  if (line.startsWith("C:")) {
    // C:<index>:<value>
    int firstColon = line.indexOf(':');
    int secondColon = line.indexOf(':', firstColon + 1);
    if (secondColon > firstColon) {
      int index = line.substring(firstColon + 1, secondColon).toInt();
      float value = line.substring(secondColon + 1).toFloat();
      if (index >= 1 && index <= NUM_GAS) {
        calibration[index - 1] = value;
        sendCalibrationLine(index, calibration[index - 1]);
      }
    }
  } else if (line.startsWith("R:SETTINGS")) {
    for (int i = 1; i <= NUM_GAS; i++) {
      sendCalibrationLine(i, calibration[i - 1]);
    }
  }
  // any other/unknown line is ignored, same as the app does on its side
}

void sendCalibrationLine(int index, float value) {
  Serial.print("C:");
  Serial.print(index);
  Serial.print(":");
  Serial.println(value, 2);
}

// ---------------------------------------------------------------------
// Simulated sensor data
// ---------------------------------------------------------------------
void sendDataLine() {
  float t = millis() / 1000.0;
  float gas[NUM_GAS];

  for (uint8_t i = 0; i < NUM_GAS; i++) {
    float phase = i * 0.9;
    float periodSeconds = 20.0 + i * 3.0;      // each channel drifts at its own rate
    float baseline = 20.0 + i * 5.0;
    float wave = 8.0 * sin(2.0 * PI * t / periodSeconds + phase);
    float noise = (random(-100, 100) / 100.0) * 1.5;   // +/- 1.5 jitter

    float reading = baseline + wave + noise + calibration[i];
    if (reading < 0) reading = 0;
    gas[i] = reading;
  }

  float temperature = 24.0 + 2.0 * sin(2.0 * PI * t / 120.0) + (random(-50, 50) / 100.0);
  float humidity = 55.0 + 5.0 * sin(2.0 * PI * t / 90.0 + 1.0) + (random(-50, 50) / 100.0);
  if (humidity < 0) humidity = 0;
  if (humidity > 100) humidity = 100;

  Serial.print("D:");
  for (uint8_t i = 0; i < NUM_GAS; i++) {
    Serial.print(gas[i], 2);
    Serial.print(",");
  }
  Serial.print(temperature, 2);
  Serial.print(",");
  Serial.println(humidity, 2);
}
