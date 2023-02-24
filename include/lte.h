#ifndef lte_h
#define lte_h

class LTEFunctions {
 public:
  bool setup();

 private:
  bool setupModem(int noAttempts);
  void enableModem();
  void disableModem();
  void resetModem();
};

#endif