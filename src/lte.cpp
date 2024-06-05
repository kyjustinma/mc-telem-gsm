
#include "lte.h"

#include <Arduino.h>
#include <utilities.h>

#include "ArduinoHttpClient.h"
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

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

TinyGsmClient client(modem);
HttpClient http(client, server, port);

util_functions utils;

bool LTEFunctions::setup() {
  pinMode(MODEM_RESET_PIN, OUTPUT);
  pinMode(BOARD_PWRKEY_PIN, OUTPUT);
  bool success = setupModem(10);

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

void LTEFunctions::getRequest(char *path) {
  Serial.print(F("Performing HTTPS GET request... "));

  // // Adjust Server Infomation if needed
  const char server[] = "https://httpbin.org";
  const char resource[] = "/ip";
  const int port = 443;
  HttpClient http(client, server, port);

  http.connectionKeepAlive();  // Currently, this is needed for HTTPS
  int err = http.get(resource);
  Serial.println("Getting Resource");
  delay(5000);
  if (err != 0) {
    Serial.println(F("failed to connect"));
    return;
  }

  int status = http.responseStatusCode();
  Serial.print(F("Response status code: "));
  Serial.println(status);
  delay(5000);
  if (!status) {
    return;
  }

  Serial.println(F("Response Headers:"));
  while (http.headerAvailable()) {
    String headerName = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    Serial.println("    " + headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0) {
    Serial.print(F("Content length is: "));
    Serial.println(length);
  }
  if (http.isResponseChunked()) {
    Serial.println(F("The response is chunked"));
  }

  String body = http.responseBody();
  Serial.println(F("Response:"));
  Serial.println(body);

  Serial.print(F("Body length is: "));
  Serial.println(body.length());

  // Adjustable server Shutdown
  // http.stop();
  // Serial.println(F("Server disconnected"));
}

// void LTEFunctions::connectMQTT() {
//   Serial.print("Connecting to ");
//   Serial.println(server);
//   if (!client.connect(server, port)) {
//     Serial.println(" fail");
//     delay(10000);
//     return;
//   }
//   Serial.println(" success");

//   // Make a HTTP GET request:
//   Serial.println("Performing HTTP GET request...");
//   client.print(String("GET ") + resource + " HTTP/1.1\r\n");
//   client.print(String("Host: ") + server + "\r\n");
//   client.print("Connection: close\r\n\r\n");
//   client.println();

//   uint32_t timeout = millis();
//   while (client.connected() && millis() - timeout < 10000L) {
//     // Print available data
//     while (client.available()) {
//       char c = client.read();
//       Serial.print(c);
//       timeout = millis();
//     }
//   }
//   Serial.println();
// }

/// @brief private functions
bool LTEFunctions::setupModem(int noAttempts) {
  bool modem_reply = false;

  // A7670 Reset modem just incase
  resetModem();
  enableModem();
  modem.begin();
  delay(10000);

  int i = noAttempts;
  Serial.println("*********");
  Serial.println("\nTesting Modem Response...\n");
  for (int i = 0; i <= noAttempts; i++) {
    Serial.printf("Modem Attempt: %d ...\n", i);
    SerialAT.println("AT");  // Probing modem
    delay(500);
    if (SerialAT.available()) {
      String r = SerialAT.readString();
      Serial.println(r);
      if (r.indexOf("OK") >= 0) {
        modem_reply = true;
        Serial.println("OK Received: Modem Connected");
        break;
        ;
      }
    }
    delay(500);
  }
  if (modem_reply == false) {
    Serial.print("Failed to get a response from the modem");
    return false;
  }
  Serial.println("*********\n");

  SimStatus sim = SIM_ERROR;
  while (sim != SIM_READY) {
    sim = modem.getSimStatus();
    switch (sim) {
      case SIM_READY:
        Serial.println("SIM card online");
        break;
      case SIM_LOCKED:
        Serial.println("The SIM card is locked. Please unlock the SIM card first.");
        // const char *SIMCARD_PIN_CODE = "123456";
        // modem.simUnlock(SIMCARD_PIN_CODE);
        break;
      default:
        break;
    }
    delay(1000);
  }

  // GPRS connection parameters are usually set after network registration
  // SETUP LTE CONNECTION

  // Serial.print(F("Connecting to "));
  // Serial.print(apn);
  // if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
  //   Serial.println(" failed");
  //   delay(10000);
  //   return false;
  // }

  Serial.println(" success");
  if (modem.isGprsConnected()) {
    Serial.print(F("Connecting to "));
    Serial.print(apn);
    Serial.println(" success");
  }
  this->getNetworkRegistration();
  delay(1000);
  this->getIp();

  String name = modem.getModemName();
  DBG("Modem Name:", name);

  String modemInfo = modem.getModemInfo();
  DBG("Modem Info:", modemInfo);
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
  delay(1000);
  digitalWrite(BOARD_PWRKEY_PIN, LOW);
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
}

void LTEFunctions::resetModem() {
  // A7670 Reset (Treset = 2min -> 2.5 typ)
  Serial.println("Reseting Modem");
  pinMode(MODEM_RESET_PIN, OUTPUT);
  digitalWrite(MODEM_RESET_PIN, LOW);
  delay(100);
  digitalWrite(MODEM_RESET_PIN, HIGH);
  delay(3000);
  digitalWrite(MODEM_RESET_PIN, LOW);
}

void LTEFunctions::getNetworkRegistration() {
  // Check network registration status and network signal status
  int16_t sq;
  Serial.print("Wait for the modem to register with the network.");
  RegStatus status = REG_NO_RESULT;
  while (status == REG_NO_RESULT || status == REG_SEARCHING || status == REG_UNREGISTERED) {
    status = modem.getRegistrationStatus();
    switch (status) {
      case REG_UNREGISTERED:
      case REG_SEARCHING:
        sq = modem.getSignalQuality();
        Serial.printf("[%lu] Signal Quality:%d\n", millis() / 1000, sq);
        delay(1000);
        break;
      case REG_DENIED:
        Serial.println("Network registration was rejected, please check if the APN is correct");
        return;
      case REG_OK_HOME:
        Serial.println("Online registration successful");
        break;
      case REG_OK_ROAMING:
        Serial.println("Network registration successful, currently in roaming mode");
        break;
      default:
        Serial.printf("Registration Status:%d\n", status);
        delay(1000);
        break;
    }
  }
  Serial.println();

  Serial.printf("Registration Status:%d\n", status);
}

void LTEFunctions::getIp() {
  // String ueInfo;
  // if (modem.getSystemInformation(ueInfo)) {
  //   Serial.print("Inquiring UE system information:");
  //   Serial.println(ueInfo);
  // }

  // if (!modem.enableNetwork()) {
  //   Serial.println("Enable network failed!");
  // }

  // delay(5000);

  String ipAddress = modem.getLocalIP();
  Serial.print("Network IP:");
  Serial.println(ipAddress);
}