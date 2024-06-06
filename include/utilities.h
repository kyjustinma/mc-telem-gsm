#pragma once

#define LILYGO_T_A7670

#if defined LILYGO_T_A7670

// #define TINY_GSM_MODEM_A7670     // From Documents
#define TINY_GSM_MODEM_SIM7600   // The AT instruction of A7670 is compatible with SIM7600
#define TINY_GSM_RX_BUFFER 1024  // Set RX buffer to 1Kb
#define DUMP_AT_COMMANDS

// Other Detailss
#define MODEM_RESET_LEVEL HIGH
#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP 600          // Time ESP32 will go to sleep (in seconds)
#define SerialMon Serial
#define SerialAT Serial1

// // Pinout
#define SERIAL_BAUDRATE 115200
#define MODEM_BAUDRATE 115200
#define MODEM_DTR_PIN 25
#define MODEM_TX_PIN 26
#define MODEM_RX_PIN 27
// The modem boot pin needs to follow the startup sequence.
#define BOARD_PWRKEY_PIN 4
#define BOARD_ADC_PIN 35
// The modem power switch must be set to HIGH for the modem to supply power.
#define BOARD_POWERON_PIN 12
#define MODEM_RING_PIN 33
#define MODEM_RESET_PIN 5
#define BOARD_MISO_PIN 2
#define BOARD_MOSI_PIN 15
#define BOARD_SCK_PIN 14
#define BOARD_SD_CS_PIN 13
#define BOARD_BAT_ADC_PIN 35
#define MODEM_GPS_ENABLE_GPIO -1

#else
#error "Use ArduinoIDE, please open the macro definition corresponding to the board above <utilities.h>"
#endif
