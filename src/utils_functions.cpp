#include "utils_functions.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Wire.h>

#include <cstring>
#include <string>

#define EEPROM_SIZE 256
// 0 - 32 = WiFi Credentials
// 32 - 128 = Gyro Calibration

#define DEBUG_N_LINE Serial.print("\n")

void util_functions::setup() {
  EEPROM.begin(EEPROM_SIZE);
  delay(2000);
}

int updateStringToEEPROM(int addrOffset, char* strToWrite) {
  EEPROM.begin(EEPROM_SIZE);
  int i = 0;
  while (strToWrite[i] != '\0') {
    if (EEPROM.read(addrOffset + 1 + i) != strToWrite[i]) {
      EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
      // Serial.printf("Writing at addr : %d char: %c\n", addrOffset + 1 +i, strToWrite[i]);
    }
    i++;
  }

  // Write the length of the char array
  byte strLength = i;
  if (EEPROM.read(addrOffset) != strLength) {
    EEPROM.write(addrOffset, strLength);  // addrOffset is the location of the data
  }
  EEPROM.commit();
  EEPROM.end();
  return addrOffset + 1 + i;  // Return the location to write for the lenght byte
}

// Reads strings from EEPROM ( length + string )
int readStringFromEEPROM(int addrOffset, char* strToRead) {
  EEPROM.begin(EEPROM_SIZE);

  byte newStrLen = EEPROM.read(addrOffset);  // Length of new string

  for (int i = 0; i < newStrLen; i++) {              // for data length
    strToRead[i] = EEPROM.read(addrOffset + 1 + i);  // Store the data (+1)
                                                     // Serial.printf("READING at addr : %d char: %c\n", addrOffset + 1 +i, strToRead[i]);
  }

  strToRead[newStrLen] = '\0';

  return addrOffset + 1 + newStrLen;  // Return the next address to read
}

// Saves the WiFi credentials at the start of EEPROM (byte 0)
void util_functions::EEPROM_write_WiFi_credentials(char* WIFI_SSID, char* WIFI_PASSWORD) {
  int eepromOffset = 0;
  // Writing
  Serial.println("---- Writing to EEPROM ----");
  int str1AddrOffset = updateStringToEEPROM(eepromOffset, WIFI_SSID);
  updateStringToEEPROM(str1AddrOffset, WIFI_PASSWORD);
  Serial.println("Saved WiFi credentials");

  // Reading
  Serial.println("---- Checking EEPROM Data ----");
  char newStr1[16];
  char newStr2[16];

  int newStr1AddrOffset = readStringFromEEPROM(eepromOffset, newStr1);
  readStringFromEEPROM(newStr1AddrOffset, newStr2);

  Serial.println(newStr1);
  Serial.println(newStr2);
  Serial.println("---- EEPROM Write Complete ----");
  delay(2000);
}

void util_functions::EEPROM_read_WiFi_credentials() {
  Serial.println("---- EEPROM read WiFi credentials ----");
  int eepromOffset = 0;
  char newStr1[16];
  char newStr2[16];

  int newStr1AddrOffset = readStringFromEEPROM(eepromOffset, newStr1);
  readStringFromEEPROM(newStr1AddrOffset, newStr2);

  Serial.println(newStr1);
  Serial.println(newStr2);
  Serial.println("---- EEPROM read complete ----");
  delay(2000);
}

void util_functions::clear_EEPROM(int addrOffset, int increments) {
  for (int i = 0; i < increments; i++) {
    EEPROM.write(addrOffset + i, 0);
  }
  EEPROM.commit();
  EEPROM.end();
  Serial.println("---- EEPROM CLEARED ---- ");
}

void read_EEPROM_WiFi_credentials(char* SSID, char* Password) {
  int SSID_Offset = 0;
  int PW_Offset = readStringFromEEPROM(SSID_Offset, SSID);
  readStringFromEEPROM(PW_Offset, Password);

  Serial.printf("\t WiFi SSID: %s \n", SSID);
  Serial.printf("\t WiFi Password: %s \n", Password);
}

// Connects to the WiFi using the SSID and WIFI password, Returns bool of connection status *BLOCKING*
bool util_functions::connect_to_wifi(char* WIFI_SSID, char* WIFI_PASSWORD, bool EEPROM_cred) {
  // We start by disconnecting to a WiFi network
  WiFi.disconnect();
  Serial.println();
  delay(100);

  for (int i = 0; i < 3; i++) {
    if (EEPROM_cred == true) {
      char EEPROM_SSID[16];
      char EEPROM_PASSWORD[16];
      read_EEPROM_WiFi_credentials(EEPROM_SSID, EEPROM_PASSWORD);
      Serial.printf("Connecting to %s", EEPROM_SSID);
      WiFi.begin(EEPROM_SSID, EEPROM_PASSWORD);
    } else {
      Serial.printf("Connecting to %s", WIFI_SSID);
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }

    uint connection_attempt = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - connection_attempt) < 30000) {
      delay(500);
      Serial.print(".");
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("");
        Serial.print("Wifi connected with IP: ");
        Serial.println(WiFi.localIP());
        Serial.println();
        return true;
      }
    }
    if (i == 1) {  // Second attempt failed back to set value
      EEPROM_cred = !EEPROM_cred;
    }
  }
  return false;
}

// Find the length of the two strings combined
int util_functions::combined_char_length(char* String1, char* String2) {
  int char_array_length = 24;
  for (byte i = 0; i < sizeof(String1) - 1; i++) {
    Serial.print(i, DEC);
    Serial.print(" = ");
    Serial.write(String1[i]);
    Serial.println();
  }
  // char_array_length = 2 + sizeof(String1) + sizeof(String2);

  return char_array_length;
}

// concatenates 2 char arrays of the form "char RemoteControl[]"
void util_functions::concat_str(char* CombinedString, char* String1, char* String2) {
  strcpy(CombinedString, String1);
  strcat(CombinedString, String2);
}

void util_functions::count_down(int duration) {
  for (int i = 0; i < duration; i++) {
    Serial.print(".");
    delay(1000);
  }
}

void check_length_charArray(char* charArray1) {
  Serial.print(sizeof(charArray1));
}
