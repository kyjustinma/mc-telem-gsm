#include "lte.h"
// GPS definitions
#define SerialAT Serial1

// General ESP32
#define TINY_GSM_MODEM_SIM7600   // The AT instruction of A7670 is compatible with SIM7600
#define TINY_GSM_RX_BUFFER 1024  // Set RX buffer to 1Kb
#define UART_BAUD 115200
#define BAT_EN 12
#define PWR_PIN 4
#define RESET 5

// Libs
#include <TinyGsmClient.h>

#include "Arduino.h"

#ifdef DUMP_AT_COMMANDS  // if enabled it requires the streamDebugger lib
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

bool lte_function::setup_lte() {
  bool success = setup_modem();

  if (success) {
    Serial.println(F("***********************************************************"));
    Serial.println(F(" You can now send AT commands"));
    Serial.println(F(" Enter \"AT\" (without quotes), and you should see \"OK\""));
    Serial.println(F(" If it doesn't work, select \"Both NL & CR\" in Serial Monitor"));
    Serial.println(F(" DISCLAIMER: Entering AT commands without knowing what they do"));
    Serial.println(F(" can have undesired consiquinces..."));
    Serial.println(F("***********************************************************\n"));
  } else {
    Serial.println(F("***********************************************************"));
    Serial.println(F(" Failed to connect to the modem! Check the baud and try again."));
    Serial.println(F("***********************************************************\n"));
  }
  return success;
}

/// @brief private functions
bool lte_function::setup_modem() {
  bool reply = false;
  pinMode(BAT_EN, OUTPUT);
  digitalWrite(BAT_EN, HIGH);

  // A7670 Reset
  modem_reset();
  modem_on();

  int i = 10;
  Serial.println("\nTesting Modem Response...\n");
  Serial.println("****");
  while (i) {
    SerialAT.println("AT");
    delay(500);
    if (SerialAT.available()) {
      String r = SerialAT.readString();
      Serial.println(r);
      if (r.indexOf("OK") >= 0) {
        reply = true;
        break;
        ;
      }
    }
    delay(500);
    i--;
  }
  Serial.println("****\n");

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  if (!modem.init()) {
    Serial.println("Failed to restart modem, attempting to continue without restarting");
    return false;
  }

  String name = modem.getModemName();
  delay(500);
  Serial.println("Modem Name: " + name);

  String modemInfo = modem.getModemInfo();
  delay(500);

  return reply;
}

void lte_function::gps_test() {
  // Disable gnss
  modem.sendAT("+CGNSSPWR=0");
  modem.waitResponse(10000L);

  // Enable gnss
  modem.sendAT("+CGNSSPWR=1");
  modem.waitResponse(10000L);

  // Wait gnss start.
  Serial.print("\tWait GPS reday.");
  while (modem.waitResponse(1000UL, "+CGNSSPWR: READY!") != 1) {
    Serial.print(".");
  }
  Serial.println();

  // Set gnss mode use GPS.
  modem.sendAT("+CGNSSMODE=1");
  modem.waitResponse(10000L);

  float parameter1, parameter2;
  char buf[16];
  while (1) {
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
        Serial.printf("lat:%s %s\n", lat, n_s);
        Serial.printf("lon:%s %s\n", lon, e_w);
        float flat = atof(lat.c_str());
        float flon = atof(lon.c_str());
        flat = (floor(flat / 100) + fmod(flat, 100.) / 60) *
               (n_s == "N" ? 1 : -1);
        flon = (floor(flon / 100) + fmod(flon, 100.) / 60) *
               (e_w == "E" ? 1 : -1);
        Serial.print("Latitude:");
        Serial.println(flat);
        Serial.print("Longitude:");
        Serial.println(flon);
      }
      break;
    } else {
      Serial.print("getGPS ");
      Serial.println(millis());
    }
    delay(2000);
  }

  // Disable gnss
  modem.sendAT("+CGNSSPWR=0");
  modem.waitResponse(10000L);
}

// Refer to https://mt-system.ru/sites/default/files/documents/a7670_series_hardware_design_v1.03.pdf for the modem spec
void lte_function::modem_on() {
  // A7670 Power On (Ton = 1s)
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(100);
  digitalWrite(PWR_PIN, HIGH);
  delay(1000);
  digitalWrite(PWR_PIN, LOW);
}

void lte_function::modem_off() {
  // A7670 Power Off (Toff = 2.5s min)
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(100);
  digitalWrite(PWR_PIN, HIGH);
  delay(3000);  // Data sheet
  digitalWrite(PWR_PIN, LOW);
}

void lte_function::modem_reset() {
  // A7670 Reset (Treset = 2min -> 2.5 typ)
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, LOW);
  delay(100);
  digitalWrite(RESET, HIGH);
  delay(3000);
  digitalWrite(RESET, LOW);
}