#ifndef gps_h
#define gps_h

class gps_functions {
 public:
  bool setup();
  void print_gps();
  void gps_test();

 private:
  bool setup_gps();
  void gps_on();
  void gps_off();
  void set_gps_mode(int modem, int output_rate);
};

#endif