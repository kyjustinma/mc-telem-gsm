#ifndef gyro_h
#define gyro_h
#include <Adafruit_BNO08x.h>

// X = Roll , Y = Pitch , Z = Yaw

struct GyroData {
  float ex, ey, ez;
  float ax, ay, az;
  float gx, gy, gz;
  bool calibrated;
};

class GyroscopeFunction {
 public:
  void setup(void);
  void printIMUData();
  void printCalibration();
  void getOrientation(GyroData* sensor_data);  // Pass variable with struct GyroData and the data will be updated in it

 private:
  void mCalculateSomething();
  void mQuaternionToEuler(float qr, float qi, float qj, float qk, GyroData* rpy, bool degrees);
  void mQuaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, GyroData* rpy, bool degrees);
  void mQuaternionToEulerGI(sh2_GyroIntegratedRV_t* rotational_vector, GyroData* rpy, bool degrees);
};

#endif