#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "WifiManager.h"
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

static const int MQ2_SAMPLE_COUNT = 8;
static const int SHARP_SAMPLE_COUNT = 4;
static const unsigned long LOOP_DELAY_MS = 10000;
static const unsigned long WIFI_RETRY_MS = 15000;

static const int CONFIG_BUTTON_PIN = 0; // GPIO0 = boton BOOT

static bool bmp180Available = false;
static bool wifiConnected = false;
static unsigned long lastWifiAttempt = 0;

/* Funciones de lectura */
static void readBmp180(float *temperatureC, float *pressurePa) {
  *temperatureC = bmp.readTemperature();
  *pressurePa = bmp.readPressure();
}

static float readMq2Voltage(void) {
  long total = 0;
  for (int i = 0; i < MQ2_SAMPLE_COUNT; i++) {
    total += analogReadMilliVolts(MQ2_ADC_PIN);
    delayMicroseconds(200);
  }
  return (float)total / MQ2_SAMPLE_COUNT / 1000.0f;
}

static float readSharpVoltage(void) {
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

void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!hasWifiConfig()) {
    Serial.println("Sin configuracion WiFi. Iniciando portal cautivo...");
    delay(500);
    startConfigPortal();
  }

  wifiConnected = setupWiFi();
  if (wifiConnected) {
    setupFirebase();
  } else {
    Serial.println("WiFi no disponible. Reintentando cada 15s.");
    Serial.println("Presiona BOOT (GPIO0) para abrir portal de configuracion.");
  }

  pinMode(CONFIG_BUTTON_PIN, INPUT_PULLUP);

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
  if (digitalRead(CONFIG_BUTTON_PIN) == LOW) {
    delay(3000);
    if (digitalRead(CONFIG_BUTTON_PIN) == LOW) {
      Serial.println("Boton presionado. Abriendo portal de configuracion...");
      startConfigPortal();
    }
  }

  if (!wifiConnected && millis() - lastWifiAttempt >= WIFI_RETRY_MS) {
    lastWifiAttempt = millis();
    Serial.println("Reintentando WiFi...");
    wifiConnected = setupWiFi();
    if (wifiConnected) {
      setupFirebase();
      Serial.println("WiFi recuperado.");
    }
  }

  static unsigned long lastSensorTime = 0;
  if (millis() - lastSensorTime >= LOOP_DELAY_MS) {
    lastSensorTime = millis();

    float temperatureC = 0.0f;
    float pressurePa = 0.0f;

    if (bmp180Available) {
      readBmp180(&temperatureC, &pressurePa);
    }

    const float mq2Voltage = readMq2Voltage();
    const float sharpVoltage = readSharpVoltage();

    if (bmp180Available) {
      Serial.printf("BMP180: OK | Temp: %.2f C | Presion: %.2f Pa\n", temperatureC, pressurePa);
    } else {
      Serial.println("BMP180: sin respuesta");
    }

    Serial.printf("MQ2: %.3f V\n", mq2Voltage);
    Serial.printf("Sharp: %.3f V\n", sharpVoltage);

    if (wifiConnected) {
      uploadSensorReadings(
        bmp180Available,
        temperatureC,
        pressurePa,
        mq2Voltage,
        sharpVoltage
      );
    }

    Serial.println();
  }

  delay(50);
}
