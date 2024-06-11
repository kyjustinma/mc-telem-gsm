#include <Arduino.h>
#include <utilities.h>

#include "I2C_data.h"
#include "TinyGsmClient.h"
#include "esp_adc_cal.h"
#include "gps.h"
#include "gyro.h"
#include "hardware_utils.h"
#include "lte.h"
#include "sd_card.h"

uint32_t timeStamp = 0;
uint32_t interval = 1000;

LTEFunctions lte;
// sd_card_function sd_card;
// GPSFunctions gps;
// sensor_Functions sensor;
// GyroscopeFunction gyroFunction;
HardwareUtils hUtils;

// GyroData mainGyroData;

void setup() {
  // Setup Serial
  Serial.begin(SERIAL_BAUDRATE);  // Set console baud rate
  Serial.println("Starting Setup...");
  delay(100);

  SerialAT.begin(MODEM_BAUDRATE, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
  pinMode(BOARD_POWERON_PIN, OUTPUT);
  digitalWrite(BOARD_POWERON_PIN, HIGH);

  Serial.print("\n\n\n\n");

  delay(100);
  // Setup hardware
  hUtils.setup();
  // sd_card.setup_sd_card();
  lte.setup(25);
  // gps.setup();
  // sensor.setup();
  // gyroFunction.setup();
}

// Main loop
void loop() {
  if (Serial.available() > 0) {
    // Read the incoming command
    String command = Serial.readStringUntil('\n');
    // Process the command
    if (command == "start") {
      // Do something when "start" is received
      Serial.println("Starting...");
    } else if (command == "stop") {
      lte.disableModem();
      // Do something when "stop" is received
      Serial.println("Stopping...");
    } else if (command == "get_request") {
      String testUrl = "https://api.ipify.org?format=json";  // Gets ip
      lte.getRequest(testUrl);
    } else if (command == "post_request") {
      String testUrl = "https://httpbin.org/post";
      lte.postRequest(testUrl, "testing post");
    } else if (command == "battery_voltage") {
      Serial.printf("Battery Voltage = %.2f \n", hUtils.getBatteryVoltage());
    } else {
      Serial.println("Unknown command: " + command);
    }
  }

  // Reads the serial input to send AT commands
  // while (true) {
  //   if (millis() - interval > 100) {
  //     GPSData gpsData = gps.getGPSData(200);
  //     interval = millis();
  //   }

  //   if (SerialAT.available()) {
  //     Serial.write(SerialAT.read());
  //   }
  //   if (Serial.available()) {
  //     SerialAT.write(Serial.read());
  //   }
  //   Serial.print(gps.getGPSString(2000));

  //   Serial.print(gpsData.lat);
  //   Serial.print(gpsData.lon);
  //   Serial.print(gpsData.time.day);

  //   sensor.getOrientation(&sensor_data, true, true);
  //   gyroFunction.getOrientation(&mainGyroData);

  //   Serial.print(mainGyroData.ex);
  //   Serial.print(mainGyroData.ey);
  //   Serial.print(mainGyroData.ez);

  //   Serial.println();
  // }
}
