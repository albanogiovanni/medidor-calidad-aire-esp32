#include "SharpSensor.h"
#include <Arduino.h>
#include "../../Config.h"

void sharpBegin() {
  pinMode(SHARP_LED_PIN, OUTPUT);
  digitalWrite(SHARP_LED_PIN, SHARP_LED_OFF_LEVEL);
  analogSetPinAttenuation(SHARP_ADC_PIN, ADC_11db);
}

// Timing segun datasheet: encender LED, esperar 280us, leer, apagar, esperar ~10ms
float sharpReadVoltage() {
  long total = 0;
  for (int i = 0; i < SHARP_SAMPLE_COUNT; i++) {
    digitalWrite(SHARP_LED_PIN, SHARP_LED_ON_LEVEL);
    delayMicroseconds(280);
    total += analogReadMilliVolts(SHARP_ADC_PIN);
    delayMicroseconds(40);
    digitalWrite(SHARP_LED_PIN, SHARP_LED_OFF_LEVEL);
    delayMicroseconds(9680);
  }
  return (float)total / SHARP_SAMPLE_COUNT / 1000.0f;
}
