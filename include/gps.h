#ifndef gps_h
#define gps_h

class gps_functions {
 public:
  void setup();
  void print_gps();

 private:
  bool setup_gps();
  void gps_on();
  void gps_off();
};

#endif