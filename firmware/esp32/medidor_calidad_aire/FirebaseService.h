#pragma once

void setupFirebase(void);

void uploadSensorReadings(
  bool bmp180Available,
  float temperatureC,
  float pressurePa,
  float mq2Voltage,
  float sharpVoltage
);
