#include <String.h>
#ifndef lte_h
#define lte_h

class LTEFunctions {
 public:
  bool setup();
  void setupGSM();
  void connectMQTT();
  void getRequest(char *path);
  void getRequest2();
  void getNetworkRegistration();
  void enabledNetwork();

  void enableModem();
  void disableModem();
  void resetModem();

  void getIp();

 private:
  bool setupModem(int noAttempts);

  // void enableModem();
  // void disableModem();
  // void resetModem();
};

#endif