

// Serial definitions
#define SerialAT Serial1
#define SerialMon Serial

// General ESP32
#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds

#define TINY_GSM_MODEM_SIM7600   // The AT instruction of A7670 is compatible with SIM7600
#define TINY_GSM_RX_BUFFER 1024  // Set RX buffer to 1Kb
#define UART_BAUD 115200
#define PIN_DTR 25
#define PIN_TX 26
#define PIN_RX 27
#define PWR_PIN 4
#define BAT_ADC 35
#define BAT_EN 12
#define PIN_RI 33
#define RESET 5

// Libs
#include "gps.h"

#include <TinyGsmClient.h>

#include "Arduino.h"

// #ifdef DUMP_AT_COMMANDS
// #include <StreamDebugger.h>
// StreamDebugger debugger(SerialAT, SerialMon);
// TinyGsm modem(debugger);
// #else
// TinyGsm modem(SerialAT);
// #endif

// void gps_functions::print_gps() {
//   float lat, lon;
//   while (1) {
//     if (modem.getGPS(&lat, &lon)) {
//       Serial.println("The location has been locked, the latitude and longitude are:");
//       Serial.print("latitude:");
//       Serial.println(lat);
//       Serial.print("longitude:");
//       Serial.println(lon);
//       break;
//     }
//     digitalWrite(BAT_EN, !digitalRead(BAT_EN));
//     Serial.println(".");
//     delay(500);
//   }
// }

// void gps_functions::gps_on() {
//   // Set SIM7000G GPIO4 LOW ,turn on GPS power
//   // CMD:AT+SGPIO=0,4,1,1
//   // Only in version 20200415 is there a function to control GPS power
//   modem.sendAT("+SGPIO=0,4,1,1");
//   if (modem.waitResponse(10000L) != 1) {
//     DBG(" SGPIO=0,4,1,1 false ");
//   }
//   modem.enableGPS();
// }

// void gps_functions::gps_off() {
//   // Set SIM7000G GPIO4 LOW ,turn off GPS power
//   // CMD:AT+SGPIO=0,4,1,0
//   // Only in version 20200415 is there a function to control GPS power
//   modem.sendAT("+SGPIO=0,4,1,0");
//   if (modem.waitResponse(10000L) != 1) {
//     DBG(" SGPIO=0,4,1,0 false ");
//   }
//   modem.disableGPS();
// }

// void gps_functions::setup() {
//   Serial.println("Start positioning . Make sure to locate outdoors.");
//   Serial.println("The blue indicator light flashes to indicate positioning.");

//   gps_on();
// }
