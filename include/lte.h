#ifndef lte_h
#define lte_h

class LTEFunctions {
 public:
  bool setupLTE();

 private:
  bool setupModem();
  void enableModem();
  void disableModem();
  void resetModem();
};

#endif