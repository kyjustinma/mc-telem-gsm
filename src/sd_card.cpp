#include "sd_card.h"
// GPS definitions
#define SerialAT Serial1

// General ESP32
#define TINY_GSM_MODEM_SIM7600   // The AT instruction of A7670 is compatible with SIM7600
#define TINY_GSM_RX_BUFFER 1024  // Set RX buffer to 1Kb
#define UART_BAUD 115200
#define PWR_PIN 4
#define BAT_EN 12
#define RESET 5

#define SD_MISO 2
#define SD_MOSI 15
#define SD_SCLK 14
#define SD_CS 13

// Libs
#include <SD.h>
#include <SPI.h>
#include <TinyGsmClient.h>

#include "Arduino.h"

bool sd_card_function::setup_sd_card() {
  bool complete = setup_spi();
  return complete;
}

/// @brief private functions
bool sd_card_function::setup_spi() {
  bool connected = false;

  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
  if (!SD.begin(SD_CS)) {
    Serial.println("> It looks like you haven't inserted the SD card..");
  } else {
    uint32_t cardSize = SD.cardSize() / (1024 * 1024);
    String str = "> SDCard Size: " + String(cardSize) + "MB";
    Serial.println(str);
  }

  return connected;
}
