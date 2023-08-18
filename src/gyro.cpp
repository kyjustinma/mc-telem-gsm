#include "gyro.h"

#include <Arduino.h>
// This demo explores two reports (SH2_ARVR_STABILIZED_RV and SH2_GYRO_INTEGRATED_RV) both can be used to give
// quartenion and euler (yaw, pitch roll) angles.  Toggle the FAST_MODE define to see other report.
// Note sensorValue.status gives calibration accuracy (which improves over time)
#include <Adafruit_BNO08x.h>

// For SPI mode, we need a CS pin
#define BNO08X_CS 10
#define BNO08X_INT 9

// #define FAST_MODE

// For SPI mode, we also need a RESET
#define BNO08X_RESET 5
// but not for I2C or UART
// #define BNO08X_RESET -1

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;
GyroData gyroData;  // Self defined data holding type

#ifdef FAST_MODE
// Top frequency is reported to be 1000Hz (but freq is somewhat variable)
sh2_SensorId_t reportType = SH2_GYRO_INTEGRATED_RV;
long reportIntervalUs = 2000;  // 2ms
#else
// Top frequency is about 250Hz but this report is more accurate
sh2_SensorId_t reportType = SH2_ARVR_STABILIZED_RV;
long reportIntervalUs = 5000;  // 5ms
static long last_read = 0;
#endif

void setReports(sh2_SensorId_t reportType, long report_interval) {
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(reportType, report_interval)) {
    Serial.println("Could not enable stabilized remote vector");
  }
}

void GyroscopeFunction::setup(void) {
  Serial.println("[SETUP]: BNO085");

  // Try to initialize!
  // if (!bno08x.begin_I2C()) { # I2C Connection
  if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {  // USING SPI
    Serial.println("Failed to find BNO085 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("BNO085 Found!");

  setReports(reportType, reportIntervalUs);

  Serial.println("Reading events");
  delay(100);
}

void GyroscopeFunction::mQuaternionToEuler(float qr, float qi, float qj, float qk, GyroData* rpy, bool degrees = true) {
  float sqr = sq(qr);
  float sqi = sq(qi);
  float sqj = sq(qj);
  float sqk = sq(qk);

  rpy->ex = atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr));  // Roll
  rpy->ey = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr));  // Pitch
  rpy->ez = atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr));   // Yaw

  if (degrees) {
    rpy->ex *= RAD_TO_DEG;
    rpy->ey *= RAD_TO_DEG;
    rpy->ez *= RAD_TO_DEG;
  }
}

void GyroscopeFunction::mQuaternionToEulerRV(sh2_RotationVectorWAcc_t* rotational_vector, GyroData* rpy, bool degrees = true) {
  mQuaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, rpy, degrees);
}

void GyroscopeFunction::mQuaternionToEulerGI(sh2_GyroIntegratedRV_t* rotational_vector, GyroData* rpy, bool degrees = true) {
  mQuaternionToEuler(rotational_vector->real, rotational_vector->i, rotational_vector->j, rotational_vector->k, rpy, degrees);
}

void GyroscopeFunction::getOrientation(GyroData* sensor_data) {
  // in this demo only one report type will be received depending on FAST_MODE define (above)
  switch (sensorValue.sensorId) {
    case SH2_ARVR_STABILIZED_RV:
      mQuaternionToEulerRV(&sensorValue.un.arvrStabilizedRV, sensor_data, true);
    case SH2_GYRO_INTEGRATED_RV:
      // faster (more noise?)
      mQuaternionToEulerGI(&sensorValue.un.gyroIntegratedRV, sensor_data, true);
      break;
  }
}

void GyroscopeFunction::printIMUData() {
  if (bno08x.wasReset()) {
    Serial.print("Sensor was reset ");
    setReports(reportType, reportIntervalUs);
  }

  if (bno08x.getSensorEvent(&sensorValue)) {
    // in this demo only one report type will be received depending on FAST_MODE define (above)
    switch (sensorValue.sensorId) {
      case SH2_ARVR_STABILIZED_RV:
        mQuaternionToEulerRV(&sensorValue.un.arvrStabilizedRV, &gyroData, true);
      case SH2_GYRO_INTEGRATED_RV:
        // faster (more noise?)
        mQuaternionToEulerGI(&sensorValue.un.gyroIntegratedRV, &gyroData, true);
        break;
    }
    long current_micro = micros();

    Serial.printf("Interval: %ld", (current_micro - last_read));
    Serial.print("\t");
    Serial.printf("Status: ", sensorValue.status);
    Serial.print("\t");  // This is accuracy in the range of 0 to 3
    Serial.printf("Data | Roll: %03.3f", gyroData.ex);
    Serial.print("\t");
    Serial.printf("Pitch: %03.3f", gyroData.ey);
    Serial.print("\t");
    Serial.printf("Yaw: %03.3f", gyroData.ez);
    Serial.println();
    last_read = current_micro;
  }
}