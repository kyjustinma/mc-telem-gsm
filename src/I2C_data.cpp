#include "I2C_data.h"

#include <Adafruit_BNO055.h>  // External IMU
#include <Adafruit_Sensor.h>  // All sensors
#include <Arduino.h>
#include <SPI.h>  // OLED (SSD1306) - Display
#include <Wire.h>

#define BNO_pin 0
#define OLED_pin 1
#define ToF_pin_front 2
#define ToF_pin_rear 3
#define Tyre_temp_pin 4

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)

#define TCAADDR 0x70

#define check_amount 3

// Variables
//  Structures and functions
//  Arduino_LSM9DS1 IMU = Arduino_LSM9DS1(); // NOT NEEDED TO initalise the internal IMU
Adafruit_BNO055 bno = Adafruit_BNO055(55);  // Access the bno055 data as bno.
sensors_event_t event;                      // Assignes for the IMU

// Variables
External_IMU E_IMU;  // IMUs BNO055

// Check_sums
unsigned long check_time = 0;
float BNO_checksum[check_amount];

// Change address on the I2C Mux (value of 0->7)
void I2C_select(uint8_t i) {
  if (i > 7) return;                // Larger than 7 doesn't exist
  Wire.beginTransmission(TCAADDR);  // Communicate change in I2C output / Input
  Wire.write(1 << i);               // Input the pin number
  Wire.endTransmission();           // End
}

// display calibration level for the IMU (BNO055)
void sensor_Functions::displayCalibrationStatus(void) {
  /* Get the four calibration values (0..3) */
  /* Any sensor data reporting 0 should be ignored, */
  /* 3 means 'fully calibrated" */
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  /* The data should be ignored until the system calibration is > 0 */
  Serial.print("\t");
  if (!system) {
    Serial.print("! ");
  }

  /* Display the individual values */
  Serial.print("Sys:");
  Serial.print(system, DEC);
  Serial.print(" G:");
  Serial.print(gyro, DEC);
  Serial.print(" A:");
  Serial.print(accel, DEC);
  Serial.print(" M:");
  Serial.println(mag, DEC);
}

// Initialise the pins and check that the Internal and External IMU are wokring
void sensor_Functions::setup() {
  // ---- Check I2C Mux is working
  Wire.begin();
  for (uint8_t t = 0; t < 8; t++) {  // Check all 8 outputs
    I2C_select(t);
    Serial.print("I2C Port #");
    Serial.println(t);  // Print which port
    for (uint8_t addr = 0; addr <= 127; addr++) {
      if (addr == TCAADDR) continue;
      Wire.beginTransmission(addr);   // Send message to see if theres a connection
      if (!Wire.endTransmission()) {  // Got return
        Serial.print("Found I2C Port #");
        Serial.println(addr, HEX);
      }
    }
  }
  Serial.println("\n\tInit|| I2C Mux");

  // Setup the I2C items
  // I2C Port 0: External IMU (BNO055)
  I2C_select(BNO_pin);
  if (!bno.begin()) {  // If theres an error then print error and loop
    Serial.println("No IMU (BNO055) detected ...");
    while (!bno.begin()) {
      delay(1000);
      Serial.print(".");
    };
  }
  bno.setExtCrystalUse(true);  // Use the external crystal to mitigate the power on reset
  displayCalibrationStatus();
  Serial.println("\n\tInit|| IMU");

  // I2C Port 2: ToF sensor (VL53L1X) - Front brakes
  // I2C_select(ToF_pin_front);
  // distanceSensor.setTimeout(500);
  // if (!distanceSensor.init()){ //Begin returns 0 on a good init
  //   Serial.println("No ToF (VL53L1X) detected ...");
  //   while (!distanceSensor.init()){
  // 	delay(1000); Serial.print(".");
  // 	}
  // }
  // distanceSensor.setDistanceMode(VL53L1X::Long);
  // distanceSensor.setMeasurementTimingBudget(50000);
  // distanceSensor.startContinuous(50);
  // Serial.println("\n\tInit|| ToF");

  // getSensorStatus.ToF_front = true;
  // getSensorStatus.ToF_rear = false;
  // update_show_init();
}

/* Calculations functions */
void calculate_IMU() {
  I2C_select(BNO_pin);
  bno.getEvent(&event);  // Get the pointer to the event for update

  // E_IMU.ex = event.orientation.z - 2.9 ; // Store the values in a struct
  // E_IMU.ey = event.orientation.y + 1.4 ; // Store the values in a struct
  // E_IMU.ez = event.orientation.x; // Store the values in a struct

  E_IMU.ax = event.acceleration.z;
  E_IMU.ay = event.acceleration.y;
  E_IMU.az = event.acceleration.x;

  E_IMU.gx = event.gyro.z;
  E_IMU.gy = event.gyro.y;
  E_IMU.gz = event.gyro.x;

  E_IMU.ex = event.orientation.z;  // Store the values in a struct
  E_IMU.ey = event.orientation.y;  // Store the values in a struct
  E_IMU.ez = event.orientation.x;  // Store the values in a struct
  E_IMU.calibrated = bno.isFullyCalibrated();

  // E_IMU.amb_temp = bno.getTemp();
}

// Return sensor data (true to calculate the data)
struct External_IMU getOrientation(bool update) {
  if (update == true) {
    calculate_IMU();
  }
  return E_IMU;
}

void sensor_Functions::save_calibration() {
  if (bno.isFullyCalibrated()) {
    Serial.println("\nFully calibrated!");
    Serial.println("--------------------------------");
    Serial.println("Calibration Results: ");
    adafruit_bno055_offsets_t newCalib;
    bno.getSensorOffsets(newCalib);
    delay(2000);
  }
}
