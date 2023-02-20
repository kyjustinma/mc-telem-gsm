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

// #ifdef DUMP_AT_COMMANDS  // if enabled it requires the streamDebugger lib
// #include <StreamDebugger.h>
// StreamDebugger debugger(SerialAT, Serial);
// TinyGsm modem(debugger);
// #else
// TinyGsm modem(SerialAT);
// #endif

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

  return reply;
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