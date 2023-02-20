

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

#include <string>

#include "Arduino.h"

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

bool gps_functions::setup() {
  // Check modem is working
  Serial.println("Checking modem...");
  if (!modem.init()) {
    Serial.println("Failed to restart modem, attempting to continue without restarting");
    return false;
  }

  String name = modem.getModemName();
  delay(500);
  Serial.println("Modem Name: " + name);
  String modemInfo = modem.getModemInfo();
  delay(500);

  Serial.println("Start positioning . Make sure to locate outdoors.");
  gps_off();
  gps_on();
  set_gps_mode(1, 1);
  return true;
}

void gps_functions::gps_on() {
  // Enable gnss
  Serial.println("Starting GPS");
  modem.sendAT("+CGNSSPWR=1,1");
  Serial.print("\tWait GPS reday.");
  while (modem.waitResponse(1000UL, "+CGNSSPWR: READY!") != 1) {
    Serial.print(".");
  }
  modem.enableGPS();
  Serial.println();
}

void gps_functions::gps_off() {
  // Disable gnss
  Serial.println("Stopping GPS");
  modem.sendAT("+CGNSSPWR=0");
  int response = modem.waitResponse(10000L);
  Serial.println(response);
  modem.disableGPS();
  Serial.println();
}

void gps_functions::set_gps_mode(int mode, int output_rate) {
  // Output rate = (1,2,4,5,10) per second

  String mode_command = "+CGNSSMODE=" + String(mode);
  modem.sendAT(mode_command);
  modem.waitResponse(10000L);
  Serial.print("Choosing GNSS Mode" + String(mode));
  // while (modem.waitResponse(1000UL, "OK") != 1) {
  //   Serial.print(".");
  // }

  String nmea_rate_command = "+CGPSNMEARATE=" + String(output_rate);
  modem.sendAT(nmea_rate_command);
  modem.waitResponse(10000L);
  Serial.print("Choosing NMEA output rate" + String(output_rate));
  // while (modem.waitResponse(1000UL, "OK") != 1) {
  //   Serial.print(".");
  // }
}

void gps_functions::gps_test() {
  float parameter1, parameter2;
  if (modem.getGPS(&parameter1, &parameter2)) {
    modem.sendAT(GF("+CGNSSINFO"));
    if (modem.waitResponse(GF(GSM_NL "+CGNSSINFO:")) == 1) {
      String res = modem.stream.readStringUntil('\n');
      String lat = "";
      String n_s = "";
      String lon = "";
      String e_w = "";
      res.trim();
      lat = res.substring(8, res.indexOf(',', 8));
      n_s = res.substring(19, res.indexOf(',', res.indexOf(',', 19)));
      lon = res.substring(21, res.indexOf(',', res.indexOf(',', 21)));
      e_w = res.substring(33, res.indexOf(',', res.indexOf(',', 33)));
      delay(100);
      Serial.println("****************GNSS********************");
      Serial.println(res);
      Serial.println(parameter1);
      Serial.println(parameter2);
      Serial.printf("lat:%s %s\n", lat, n_s);
      Serial.printf("lon:%s %s\n", lon, e_w);
      float flat = atof(lat.c_str());
      float flon = atof(lon.c_str());
      flat = (floor(flat / 100) + fmod(flat, 100.) / 60) * (n_s == "N" ? 1 : -1);
      flon = (floor(flon / 100) + fmod(flon, 100.) / 60) * (e_w == "E" ? 1 : -1);
      Serial.print("Latitude:");
      Serial.println(flat);
      Serial.print("Longitude:");
      Serial.println(flon);
    }
  } else {
    Serial.print("getGPS ");
    Serial.println(millis());
  }
}
