#ifndef utils_functions_h
#define utils_functions_h

/*
Utliitiy folder for the Tiny Pico system

*/

struct WiFi_Credential {
  char SSID[16];
  char Password[16];
  bool store_SSID;
  bool store_Password;
};

class util_functions {
 public:
  void setup();

  bool connect_to_wifi(char* SSID, char* Password, bool EEPROM_cred);

  void EEPROM_write_WiFi_credentials(char* WIFI_SSID, char* WIFI_PASSWORD);
  void EEPROM_read_WiFi_credentials();  // Print

  void clear_EEPROM(int addrOffset, int increment);

  int combined_char_length(char* String1, char* String2);
  void concat_str(char* CombinedString, char* String1, char* String2);

  void count_down(int duration);
};

#endif