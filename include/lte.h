#ifndef lte_h
#define lte_h

class LTEFunctions {
 public:
  bool setup();
  void connectMQTT();
  void getRequest(char *path);

 private:
  bool setupModem(int noAttempts);
  void enableModem();
  void disableModem();
  void resetModem();
};

#endif