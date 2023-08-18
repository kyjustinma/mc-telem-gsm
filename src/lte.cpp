#include "lte.h"
// GPS definitions
#define TINY_GSM_MODEM_SIM7600
#define SerialAT Serial1  // Connected to LTE module

// General ESP32
#define TINY_GSM_MODEM_SIM7600   // The AT instruction of A7670 is compatible with SIM7600
#define TINY_GSM_RX_BUFFER 1024  // Set RX buffer to 1Kb
#define UART_BAUD 115200
#define BAT_EN 12
#define PWR_PIN 4
#define RESET 5

// Libs
#include <ArduinoHttpClient.h>
#include <TinyGsmClient.h>

#include "Arduino.h"
#include "utils_functions.h"

// Your GPRS credentials, if any
const char apn[] = "YourAPN";
const char gprsUser[] = "";
const char gprsPass[] = "";

// Server Infomation
const char server[] = "https://httpbin.org";
const int port = 443;

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
HttpClient http(client, server, port);

util_functions utils;

bool LTEFunctions::setup() {
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
  // const char server[] = "https://httpbin.org";
  // const char resource[] = "/ip";
  // const int port = 443;
  // HttpClient http(client, server, port);

  http.connectionKeepAlive();  // Currently, this is needed for HTTPS

  int err = http.get(path);
  if (err != 0) {
    Serial.println(F("failed to connect"));
    delay(5000);
    return;
  }

  int status = http.responseStatusCode();
  Serial.print(F("Response status code: "));
  Serial.println(status);
  if (!status) {
    delay(5000);
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

  int i = noAttempts;
  Serial.println("*********");
  Serial.println("\nTesting Modem Response...\n");
  while (i) {
    SerialAT.println("AT");  // Probing modem
    delay(500);
    if (SerialAT.available()) {
      String r = SerialAT.readString();
      Serial.println(r);
      if (r.indexOf("OK") >= 0) {
        modem_reply = true;
        break;
        ;
      }
    }
    delay(500);
    i--;
  }
  Serial.println("*********\n");

  // GPRS connection parameters are usually set after network registration
  // SETUP LTE CONNECTION
  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println(" fail");
    delay(10000);
    return false;
  }
  Serial.println(" success");
  if (modem.isGprsConnected()) {
    Serial.println("GPRS connected");
  }

  return true;
}

// Refer to https://mt-system.ru/sites/default/files/documents/a7670_series_hardware_design_v1.03.pdf for the modem spec
void LTEFunctions::enableModem() {
  // A7670 Power On (Ton = 1s)
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(100);
  digitalWrite(PWR_PIN, HIGH);
  delay(1000);
  digitalWrite(PWR_PIN, LOW);
}

void LTEFunctions::disableModem() {
  // A7670 Power Off (Toff = 2.5s min)
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);
  delay(100);
  digitalWrite(PWR_PIN, HIGH);
  delay(3000);  // Data sheet
  digitalWrite(PWR_PIN, LOW);
}

void LTEFunctions::resetModem() {
  // A7670 Reset (Treset = 2min -> 2.5 typ)
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, LOW);
  delay(100);
  digitalWrite(RESET, HIGH);
  delay(3000);
  digitalWrite(RESET, LOW);
}