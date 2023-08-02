/*
  FILE: ATdebug.ino
  AUTHOR: Kaibin
  PURPOSE: ATdebug functionality
*/

#define TINY_GSM_MODEM_SIM7600   // The AT instruction of A7670 is compatible with SIM7600
#define TINY_GSM_RX_BUFFER 1024  // Set RX buffer to 1Kb
#define SerialAT Serial1

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 600          // Time ESP32 will go to sleep (in seconds)

#define UART_BAUD 115200
#define PIN_DTR 25
#define PIN_TX 26
#define PIN_RX 27
#define PWR_PIN 4
#define BAT_ADC 35
#define BAT_EN 12
#define PIN_RI 33
#define RESET 5

#define SD_MISO 2
#define SD_MOSI 15
#define SD_SCLK 14
#define SD_CS 13

#include <TinyGsmClient.h>

#include "Arduino.h"
#include "I2C_data.h"
#include "gps.h"
#include "lte.h"
#include "sd_card.h"

bool reply = false;
static unsigned long interval = 0;

LTEFunctions lte;
sd_card_function sd_card;
GPSFunctions gps;
sensor_Functions sensor;

void setup() {
  // Setup Serial
  Serial.begin(UART_BAUD);  // Set console baud rate
  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
  Serial.print("\n\n\n\n");

  while (!Serial) {
    delay(1);
  }
  delay(100);

  // Setup hardware
  sd_card.setup_sd_card();
  lte.setup();
  // gps.setup();
  sensor.setup();
}

// Main loop
void loop() {
  // Reads the serial input to send AT commands
  while (true) {
    // if (millis() - interval > 100) {
    // GPSData testing = gps.getGPSData(200);
    // interval = millis();
    // }

    // if (SerialAT.available()) {
    //   Serial.write(SerialAT.read());
    // }
    // if (Serial.available()) {
    //   SerialAT.write(Serial.read());
    // }
    // Serial.print(testing.lat);
    // Serial.print(testing.lon);
    // Serial.print(gps.getGPSString(2000));
    Serial.print(getOrientation(true).ex);
    Serial.print(getOrientation(true).ey);
    Serial.print(getOrientation(true).ez);
    Serial.println();
    delay(50);
  }
}