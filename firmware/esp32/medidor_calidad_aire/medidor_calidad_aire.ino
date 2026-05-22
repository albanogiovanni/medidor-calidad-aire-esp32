#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "WifiConnection.h"
#include "FirebaseService.h"

/* Pines y sensores */
static const int BMP180_SDA_PIN = 21;
static const int BMP180_SCL_PIN = 22;
static Adafruit_BMP085 bmp;

static const int MQ2_ADC_PIN = 34;
static const int SHARP_ADC_PIN = 35;
static const int SHARP_LED_PIN = 25;
static const int SHARP_LED_ON_LEVEL = LOW;
static const int SHARP_LED_OFF_LEVEL = HIGH;

static const float ADC_MAX_READING = 4095.0f;
static const float ADC_VREF_ESTIMATE = 3.3f;
static const int MQ2_SAMPLE_COUNT = 8;
static const unsigned long LOOP_DELAY_MS = 5000;

static bool bmp180Available = false;

/* Funciones de lectura */
static float adcRawToVoltage(int raw) {
  return (raw / ADC_MAX_READING) * ADC_VREF_ESTIMATE;
}

static void readBmp180(float *temperatureC, float *pressurePa) {
  *temperatureC = bmp.readTemperature();
  *pressurePa = bmp.readPressure();
}

static int readMq2Raw(void) {
  int total = 0;
  for (int i = 0; i < MQ2_SAMPLE_COUNT; i++) {
    total += analogRead(MQ2_ADC_PIN);
    delayMicroseconds(200);
  }
  return total / MQ2_SAMPLE_COUNT;
}

static int readSharpRaw(void) {
  int raw = 0;
  digitalWrite(SHARP_LED_PIN, SHARP_LED_ON_LEVEL);
  delayMicroseconds(280);
  raw = analogRead(SHARP_ADC_PIN);
  delayMicroseconds(40);
  digitalWrite(SHARP_LED_PIN, SHARP_LED_OFF_LEVEL);
  delayMicroseconds(9680);
  return raw;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  connectWiFi();
  setupFirebase();

  /* BMP180 */
  Wire.begin(BMP180_SDA_PIN, BMP180_SCL_PIN);
  bmp180Available = bmp.begin();

  if (!bmp180Available) {
    Serial.println("BMP180 no encontrado!");
  }

  /* SHARP */
  pinMode(SHARP_LED_PIN, OUTPUT);
  digitalWrite(SHARP_LED_PIN, SHARP_LED_OFF_LEVEL);

  analogReadResolution(12);
  analogSetPinAttenuation(MQ2_ADC_PIN, ADC_11db);
  analogSetPinAttenuation(SHARP_ADC_PIN, ADC_11db);

  Serial.println("Sistema listo.");
}

void loop() {
  float temperatureC = 0.0f;
  float pressurePa = 0.0f;

  if (bmp180Available) {
    readBmp180(&temperatureC, &pressurePa);
  }

  const int mq2Raw = readMq2Raw();
  const int sharpRaw = readSharpRaw();
  const float mq2Voltage = adcRawToVoltage(mq2Raw);
  const float sharpVoltage = adcRawToVoltage(sharpRaw);

  if (bmp180Available) {
    Serial.printf("BMP180: OK | Temp: %.2f C | Presion: %.2f Pa\n", temperatureC, pressurePa);
  } else {
    Serial.println("BMP180: sin respuesta");
  }

  Serial.printf("MQ2: ADC %d | Voltaje aprox: %.2f V\n", mq2Raw, mq2Voltage);
  Serial.printf("Sharp: ADC %d | Voltaje aprox: %.2f V\n", sharpRaw, sharpVoltage);

  uploadSensorReadings(
    bmp180Available,
    temperatureC,
    pressurePa,
    mq2Raw,
    mq2Voltage,
    sharpRaw,
    sharpVoltage
  );

  Serial.println();

  delay(LOOP_DELAY_MS);
}
