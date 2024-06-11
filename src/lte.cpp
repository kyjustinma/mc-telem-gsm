
#include "lte.h"

#include <Arduino.h>
#include <utilities.h>

#include "TinyGsmClient.h"
#include "utils_functions.h"

// Your GPRS credentials, if any
const char apn[] = "CMHK";
const char gprsUser[] = "";
const char gprsPass[] = "";
// Server Infomation
const char server[] = "vsh.pp.ua";
const char resource[] = "/TinyGSM/logo.txt";
const int port = 80;

const unsigned long setupTimeout = 30000;

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

TinyGsmClient client(modem);

util_functions utils;

bool LTEFunctions::setup(int noAttempts) {
  pinMode(MODEM_RESET_PIN, OUTPUT);
  pinMode(BOARD_PWRKEY_PIN, OUTPUT);
  bool success = false;
  resetModem();
  enableModem();
  testModem(noAttempts);
  getSimStatus();
  success = getNetworkRegistration();
  getIp();
  return success;
}

bool LTEFunctions::getRequest(String url_endpoint) {
  // Initialize HTTPS
  modem.https_begin();

  // Set GET URT
  if (!modem.https_set_url(url_endpoint)) {
    Serial.println("Failed to set the URL. Please check the validity of the URL!");
    return false;
  }

  // Send GET request
  int httpCode = 0;
  httpCode = modem.https_get();
  if (httpCode != 200) {
    Serial.print("HTTP get failed ! error code = ");
    Serial.println(httpCode);
    return true;
  }

  // Get HTTPS header information
  String header = modem.https_header();
  Serial.print("HTTP Header : ");
  Serial.println(header);

  delay(5000);

  // Get HTTPS response
  String body = modem.https_body();
  Serial.print("HTTP body : ");
  Serial.println(body);

  modem.https_end();
}

bool LTEFunctions::postRequest(String url_endpoint, String data) {
  // Initialize HTTPS
  modem.https_begin();

  // Set GET URT
  if (!modem.https_set_url(url_endpoint)) {
    Serial.println("Failed to set the URL. Please check the validity of the URL!");
    return false;
  }

  //
  modem.https_add_header("Accept-Language", "zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6");
  modem.https_add_header("Accept-Encoding", "gzip, deflate, br");
  modem.https_set_accept_type("application/json");
  modem.https_set_user_agent("TinyGSM/LilyGo-A76XX");

  int httpCode = modem.https_post(data);
  if (httpCode != 200) {
    Serial.print("HTTP post failed ! error code = ");
    Serial.println(httpCode);
    return false;
  }

  // Get HTTPS header information
  String header = modem.https_header();
  Serial.print("HTTP Header : ");
  Serial.println(header);

  // Get HTTPS response
  String body = modem.https_body();
  Serial.print("HTTP body : ");
  Serial.println(body);
  return true;
}

// Refer to https://mt-system.ru/sites/default/files/documents/a7670_series_hardware_design_v1.03.pdf for the modem spec
void LTEFunctions::enableModem() {
  // A7670 Power On (Ton = 1s)
  Serial.println("Enabling Modem");
  pinMode(BOARD_PWRKEY_PIN, OUTPUT);
  digitalWrite(BOARD_PWRKEY_PIN, LOW);
  delay(100);
  digitalWrite(BOARD_PWRKEY_PIN, HIGH);
  delay(100);
  digitalWrite(BOARD_PWRKEY_PIN, LOW);
  Serial.println("Modem: Enabled");
}

void LTEFunctions::disableModem() {
  // A7670 Power Off (Toff = 2.5s min)
  Serial.println("Disabling Modem");
  pinMode(BOARD_PWRKEY_PIN, OUTPUT);
  digitalWrite(BOARD_PWRKEY_PIN, LOW);
  delay(100);
  digitalWrite(BOARD_PWRKEY_PIN, HIGH);
  delay(3000);  // Data sheet
  digitalWrite(BOARD_PWRKEY_PIN, LOW);
  Serial.println("Modem: Disabled");
}

void LTEFunctions::resetModem() {
  // A7670 Reset (Treset = 2min -> 2.5 typ)
  Serial.println("Reseting Modem");
  pinMode(MODEM_RESET_PIN, OUTPUT);
  digitalWrite(MODEM_RESET_PIN, LOW);
  delay(100);
  digitalWrite(MODEM_RESET_PIN, HIGH);
  delay(2600);
  digitalWrite(MODEM_RESET_PIN, LOW);
  Serial.println("Modem: Reset");
}

bool LTEFunctions::testModem(int noAttempts) {
  Serial.println("\nTesting Modem Response...\n");
  for (int i = 0; i <= noAttempts; i++) {
    Serial.printf("testModem Attempt: %d ...\n", i);
    SerialAT.println("AT");  // Probing modem
    if (SerialAT.available()) {
      String r = SerialAT.readString();
      if (r.indexOf("OK") >= 0) {
        Serial.println("Modem: Connected");
        return true;
        break;
        ;
      }
    }
    delay(1000);
  }
  Serial.println("Failed to get a response from the modem");
  return false;
}

bool LTEFunctions::getSimStatus() {
  // Check if SIM card is online
  SimStatus sim = SIM_ERROR;
  unsigned long timeoutStart = millis();
  while (sim != SIM_READY && millis() - timeoutStart < 10000) {
    sim = modem.getSimStatus();
    switch (sim) {
      case SIM_READY:
        Serial.println("Modem: SIM card online");
        return true;
        break;
      case SIM_LOCKED:
        Serial.println("Modem: SIM card locked. Please unlock the SIM card first.");
        // const char *SIMCARD_PIN_CODE = "123456";
        // modem.simUnlock(SIMCARD_PIN_CODE);
        break;
      default:
        Serial.println("Modem: SIM card offline");
        break;
    }
    delay(1000);
  }
  return false;
}

bool LTEFunctions::getNetworkRegistration() {
  if (!modem.setNetworkMode(MODEM_NETWORK_AUTO)) {
    Serial.println("Modem: Failed to set network mode.");
  }
  String mode = modem.getNetworkModes();
  Serial.print("Modem: Current network mode = ");
  Serial.println(mode);

  // Check network registration status and network signal status
  Serial.println("Waiting for the modem to register with the network.");
  bool networkRegistered = false;
  int16_t sq;
  RegStatus status = REG_NO_RESULT;
  unsigned long timeoutStart = millis();
  while ((status == REG_NO_RESULT || status == REG_UNREGISTERED || status == REG_SEARCHING) && (millis() - timeoutStart < setupTimeout)) {
    status = modem.getRegistrationStatus();
    switch (status) {
      case REG_DENIED:
        Serial.println("Modem: Network registration was rejected, please check if the APN is correct");
        return false;
      case REG_NO_RESULT:
        networkRegistered = false;
      case REG_UNREGISTERED:
        networkRegistered = false;
      case REG_SEARCHING:
        sq = modem.getSignalQuality();
        Serial.printf("[%lu] Signal Quality:%d\n", (millis() - timeoutStart) / 1000, sq);  // Prints the time pass so far and the signal quality
        networkRegistered = false;
        delay(1000);
        break;
      case REG_OK_HOME:
        Serial.println("Mode: Online registration successful");
        networkRegistered = true;
        break;
      case REG_OK_ROAMING:
        Serial.println("Modem: Network registration successful, currently in roaming mode");
        networkRegistered = true;
        break;
      default:
        Serial.printf("Registration Status:%d\n", status);
        delay(1000);
        break;
    }
  }
  Serial.println();
  Serial.printf("Registration Status:%d\n", status);

  if (!modem.enableNetwork()) {
    Serial.println("Modem: Failed to enable network.");
    return false;
  }
  Serial.println("Modem: Network enabled.");
  return networkRegistered;
}

String LTEFunctions::getIp() {
  String ipAddress = modem.getLocalIP();
  Serial.print("Network IP:");
  Serial.println(ipAddress);
  return ipAddress;
}