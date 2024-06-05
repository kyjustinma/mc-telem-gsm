#ifndef hardware_utils_h
#define hardware_utils_h

/*
Utliitiy folder for the Tiny Pico system

*/

class HardwareUtils {
 public:
  void setup();
  float getBatteryVoltage();
};

#endif