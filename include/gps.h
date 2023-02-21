#ifndef gps_h
#define gps_h

class GPSFunctions {
 public:
  bool setup();
  void printGPS();
  void testGPS();

 private:
  bool setupGPS();
  void enableGPS();
  void disableGPS();
  void setGPSMode(int modem, int output_rate);
  String getGPSString(int interval);
  GPSData getGPSData();
};

struct GPSTime {
  int year, month, day;
  int hours, minutes, seconds, milliseconds;
  char timeString[16];
};

struct GPSData {
  int fix;
  double lat, lon, alt;
  GPSTime time;
  double speed;
  int quality;
  double course;
};

#endif