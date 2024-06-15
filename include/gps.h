#include <string>

#include "Arduino.h"

#ifndef gps_h
#define gps_h

struct GPSTime {
  int year, month, day;
  int hours, minutes, seconds, milliseconds;
  char timeString[16];
};

struct GPSData {
  uint8_t status;
  float lat, lon, speed, alt;
  int vsat, usat;
  float accuracy;
  int quality;
  int fix;
  GPSTime time;
  double course;
};
// struct GPSData {
//   int fix;
//   double lat, lon, alt;
//   GPSTime time;
//   double speed;
//   int quality;
//   double course;
// };

void convertGPSData(String CGNSSINFO);
char *strtoke(char *str, const char *delim);
class GPSFunctions {
 public:
  bool setup();
  void printGPS();
  void testGPS();
  String getGPSString(int interval);
  GPSData getGPSData(int interval);
  void getDateTime();

 private:
  bool
  setupGPS();
  void enableGPS();
  void disableGPS();
  void setGPSMode(int modem, int output_rate);
};

#endif