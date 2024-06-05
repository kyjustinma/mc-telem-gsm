
#include "hardware_utils.h"

#include <Arduino.h>
#include <esp_adc_cal.h>
#include <string.h>
#include <utilities.h>

void HardwareUtils::setup() {
  // pinMode(BOARD_POWERON_PIN, OUTPUT);
  // digitalWrite(BOARD_POWERON_PIN, HIGH);
}

// concatenates 2 char arrays of the form "char RemoteControl[]"
float HardwareUtils::getBatteryVoltage() {
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  uint16_t battery_voltage = esp_adc_cal_raw_to_voltage(analogRead(BOARD_BAT_ADC_PIN), &adc_chars) * 2;
  Serial.printf("Internal Voltage = %.2f \n", battery_voltage);
  float true_voltage = float(battery_voltage) / 1012.38;
  return true_voltage;
}
