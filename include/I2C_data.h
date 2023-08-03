#ifndef I2C_data_h
#define I2C_data_h

struct External_IMU {
  double ex, ey, ez;
  double ax, ay, az;
  double gx, gy, gz;
  double amb_temp;

  bool calibrated;
};
class sensor_Functions {
 public:
  void setup();
  void check_sensors();
  void displayCalibrationStatus(void);

  void show_init_OLED(char* sensor, bool status = false, bool confirmed = false, bool other_text = true);
  void update_show_init();
  void OLED_display(char* text, int size);

  void print_IMU();

  void save_calibration();

  void getOrientation(External_IMU* sensor_data, bool update, bool print);
};

// struct External_IMU getOrientation(bool update);
// struct Time_oF getSuspension(bool update);
// struct Tyre_temp getTyreTemp(bool update);

#endif