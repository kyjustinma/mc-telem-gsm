#ifndef lte_h
#define lte_h

class lte_function {
 public:
  bool setup_lte();
  void gps_test();

 private:
  bool setup_modem();
  void modem_on();
  void modem_off();
  void modem_reset();
};

#endif