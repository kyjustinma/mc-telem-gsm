

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

#include <map>
#include <string>

#include "Arduino.h"

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modemGPS(debugger);
#else
TinyGsm modemGPS(SerialAT);
#endif
GPSData internalGPSData;

bool GPSFunctions::setup() {
  // Check modem is working
  Serial.println("Checking if modemGPS chip is connected");
  if (!modemGPS.init()) {
    Serial.println("Failed to restart modemGPS, attempting to continue without restarting");
    return false;
  }

  Serial.printf("ModemGPS Name: %s \nModemGPS Info: %s \n", modemGPS.getModemName().c_str(), modemGPS.getModemInfo().c_str());
  Serial.println("Start positioning . Make sure to locate outdoors.");
  disableGPS();
  enableGPS();
  setGPSMode(3, 5);
  return true;
}

void GPSFunctions::enableGPS() {
  // Enable gnss
  modemGPS.sendAT("+CGNSSPWR=1,1");
  while (modemGPS.waitResponse(1000UL, "+CGNSSPWR: READY!") != 1) {
    Serial.print(".");
  }
  modemGPS.enableGPS();
}

void GPSFunctions::disableGPS() {
  // Disable gnss
  modemGPS.sendAT("+CGNSSPWR=0");
  // while (modemGPS.waitResponse(1000UL, "+CGNSSPWR: READY!") != 1) {
  //   Serial.print(".");
  // }
  modemGPS.disableGPS();
}

void GPSFunctions::setGPSMode(int mode, int output_rate) {
  // Mode = 1[GPS] 3[GPS + BDS]
  // Output rate = (1,2,4,5,10) per second
  char mode_command[16];
  char output_command[16];

  sprintf(mode_command, "+CGNSSMODE=%i", mode);
  Serial.printf("Choosing GNSS Mode [%s]", mode_command);
  modemGPS.sendAT(mode_command);
  // Serial.println(modemGPS.waitResponse(1000UL, "OK"));
  while (modemGPS.waitResponse(10000L, "OK") != 1) {
    Serial.print(".");
  }

  sprintf(output_command, "+CGPSNMEARATE=%i", output_rate);
  Serial.printf("Choosing NMEA Output Rate [%s]", output_command);
  modemGPS.sendAT(output_command);
  while (modemGPS.waitResponse(10000L, "OK") != 1) {
    Serial.print(".");
  }
}

String GPSFunctions::getGPSString(int interval = 1000) {
  /* AT+CGNSSINFO
  +CGNSSINFO: [<mode>],[<GPS-SVs>],[<GLONASS-SVs>],[BEIDOU-SVs],
  [<lat>],[<N/S>],[<log>],[<E/W>],[<date>],[<UTC-time>],
  [<alt>],[<speed>],[<course>],[<PDOP>],[HDOP],[VDOP]
  */
  static unsigned long gps_last_received = 0;
  String response = "NA";

  float parameter1, parameter2;
  if (modemGPS.getGPS(&parameter1, &parameter2) && (millis() - gps_last_received) > interval) {
    modemGPS.sendAT(GF("+CGNSSINFO"));
    if (modemGPS.waitResponse(GF(GSM_NL "+CGNSSINFO:")) == 1) {
      response = modemGPS.stream.readStringUntil('\n');
      Serial.print("Response: ");
      Serial.println(response);
      gps_last_received = millis();
    }
  }
  return response;
}

GPSData GPSFunctions::getGPSData(int interval = 1000) {
  String NMEA_data = getGPSString(interval);
  if (NMEA_data != "NA") {
    convertGPSData(NMEA_data);
    return internalGPSData;
  }
  return internalGPSData;
}

void convertGPSData(String CGNSSINFO) {
  /* AT+CGNSSINFO
  +CGNSSINFO: [<mode>],[<GPS-SVs>],[<GLONASS-SVs>],[BEIDOU-SVs],
  [<lat>],[<N/S>],[<log>],[<E/W>],[<date>],[<UTC-time>],
  [<alt>],[<speed>],[<course>],[<PDOP>],[HDOP],[VDOP]
  */
  char *strings[32];  // an array of pointers to the pieces of the above array after strtok()
  char *ptr = NULL;
  // char res[128];
  int response_length = CGNSSINFO.length() + 1;
  char response[response_length];
  CGNSSINFO.toCharArray(response, response_length);

  byte index = 0;
  ptr = strtoke(response, ",");  // delimiter
  while (ptr != NULL) {
    strings[index] = ptr;
    index++;
    ptr = strtoke(NULL, ",");
  }

  // Serial.println("The Pieces separated by strtok()");
  // for (int n = 0; n < index; n++) {
  //   Serial.print(n);
  //   Serial.print("  ");
  //   Serial.println(strings[n]);
  // }

  internalGPSData.fix = atof(strings[1]);
  float fLat = atof(strings[5]);
  internalGPSData.lat = (floor(fLat / 100) + fmod(fLat, 100.) / 60) * (strcmp(strings[6], "N") ? 1 : -1);
  float fLon = atof(strings[7]);
  internalGPSData.lon = (floor(fLon / 100) + fmod(fLon, 100.) / 60) * (strcmp(strings[6], "E") ? 1 : -1);
  char dateTemp[3];
  strncpy(dateTemp, strings[9], 2);
  internalGPSData.time.day = atoi(dateTemp);
  strncpy(dateTemp, strings[9] + 2, 2);
  internalGPSData.time.day = atoi(dateTemp);
  strncpy(dateTemp, strings[9] + 4, 2);
  internalGPSData.time.year = 2000 + atoi(dateTemp);  // year 2000 + XX
  sprintf(internalGPSData.time.timeString, "%s", strings[10]);
  internalGPSData.alt = atof(strings[11]);
  internalGPSData.speed = atof(strings[12]);
  internalGPSData.course = atof(strings[13]);

  Serial.print(internalGPSData.lat);
  Serial.print("\t");
  Serial.println(internalGPSData.lon);
}

// Converting NMEA to array of data
char *strtoke(char *str, const char *delim) {
  static char *start = NULL; /* stores string str for consecutive calls */
  char *token = NULL;        /* found token */
  /* assign new start in case */
  if (str) start = str;
  /* check whether text to parse left */
  if (!start) return NULL;
  /* remember current start as found token */
  token = start;
  /* find next occurrence of delim */
  start = strpbrk(start, delim);
  /* replace delim with terminator and move start to follower */
  if (start) *start++ = '\0';
  /* done */
  return token;
}

void GPSFunctions::testGPS() {
  float parameter1, parameter2;
  if (modemGPS.getGPS(&parameter1, &parameter2)) {
    modemGPS.sendAT(GF("+CGNSSINFO"));
    if (modemGPS.waitResponse(GF(GSM_NL "+CGNSSINFO:")) == 1) {
      String res = modemGPS.stream.readStringUntil('\n');
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
      Serial.printf("lat:%s %s\n", lat.c_str(), n_s.c_str());
      Serial.printf("lon:%s %s\n", lon.c_str(), e_w.c_str());
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
    // Serial.print("getGPS ");
    // Serial.println(millis());
    Serial.print(".");
  }
}