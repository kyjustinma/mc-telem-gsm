#include <Arduino.h>
#ifndef lte_h
#define lte_h

struct ModemStatus {
  bool enabled = false;
  bool connected = false;
  bool simConnected = false;
  bool networkRegistered = false;
  bool ip = false;
};

class LTEFunctions {
 public:
  bool setup(int noAttempts);
  void connectMQTT();
  bool getRequest(String url_endpoint);
  bool postRequest(String url_endpoint, String data);

  // Hardware control of the modem
  void enableModem();
  void disableModem();
  void resetModem();
  bool testModem(int noAttempts);

  bool getSimStatus();
  bool getNetworkRegistration();
  void enabledNetwork();
  String getIp();

 private:
  // void enableModem();
  // void disableModem();
  // void resetModem();
};

#endif