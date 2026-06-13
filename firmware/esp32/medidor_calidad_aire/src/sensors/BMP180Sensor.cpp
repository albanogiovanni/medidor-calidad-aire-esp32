#include "BMP180Sensor.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "../../Config.h"

static Adafruit_BMP085 bmp;
static bool available = false;

void bmp180Begin() {
  Wire.begin(BMP180_SDA_PIN, BMP180_SCL_PIN);
  available = bmp.begin();
  if (!available) {
    Serial.println("BMP180 no encontrado!");
  }
}

bool bmp180IsAvailable() {
  return available;
}

void bmp180Read(float *temperatureC, float *pressurePa) {
  *temperatureC = bmp.readTemperature();
  *pressurePa = bmp.readPressure();
}
