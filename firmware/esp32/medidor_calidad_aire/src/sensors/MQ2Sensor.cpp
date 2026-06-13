#include "MQ2Sensor.h"
#include <Arduino.h>
#include "../../Config.h"

void mq2Begin() {
  analogSetPinAttenuation(MQ2_ADC_PIN, ADC_11db);
}

float mq2ReadVoltage() {
  long total = 0;
  for (int i = 0; i < MQ2_SAMPLE_COUNT; i++) {
    total += analogReadMilliVolts(MQ2_ADC_PIN);
    delayMicroseconds(200);
  }
  return (float)total / MQ2_SAMPLE_COUNT / 1000.0f;
}
