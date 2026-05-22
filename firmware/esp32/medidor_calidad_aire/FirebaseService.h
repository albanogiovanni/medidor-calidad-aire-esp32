#pragma once

void setupFirebase(void);

void uploadSensorReadings(
  bool bmp180Available,
  float temperatureC,
  float pressurePa,
  int mq2Raw,
  float mq2Voltage,
  int sharpRaw,
  float sharpVoltage
);
