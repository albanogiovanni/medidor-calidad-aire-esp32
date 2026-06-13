#include "Config.h"
#include "src/sensors/BMP180Sensor.h"
#include "src/sensors/MQ2Sensor.h"
#include "src/sensors/SharpSensor.h"
#include "src/services/WifiManager.h"
#include "src/services/FirebaseService.h"

static bool wifiConnected = false;
static unsigned long lastWifiAttempt = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Si no hay WiFi guardado, abrir portal de configuracion
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

  bmp180Begin();
  mq2Begin();
  sharpBegin();

  analogReadResolution(12);

  Serial.println("Sistema listo.");
}

void loop() {
  // Boton BOOT: mantener 3s para abrir portal de configuracion
  if (digitalRead(CONFIG_BUTTON_PIN) == LOW) {
    delay(3000);
    if (digitalRead(CONFIG_BUTTON_PIN) == LOW) {
      Serial.println("Boton presionado. Abriendo portal de configuracion...");
      startConfigPortal();
    }
  }

  // Reintentar WiFi periodicamente
  if (!wifiConnected && millis() - lastWifiAttempt >= WIFI_RETRY_MS) {
    lastWifiAttempt = millis();
    Serial.println("Reintentando WiFi...");
    wifiConnected = setupWiFi();
    if (wifiConnected) {
      setupFirebase();
      Serial.println("WiFi recuperado.");
    }
  }

  // Leer sensores y enviar datos cada LOOP_DELAY_MS
  static unsigned long lastSensorTime = 0;
  if (millis() - lastSensorTime >= LOOP_DELAY_MS) {
    lastSensorTime = millis();

    float temperatureC = 0.0f;
    float pressurePa = 0.0f;

    if (bmp180IsAvailable()) {
      bmp180Read(&temperatureC, &pressurePa);
      Serial.printf("BMP180: OK | Temp: %.2f C | Presion: %.2f Pa\n", temperatureC, pressurePa);
    } else {
      Serial.println("BMP180: sin respuesta");
    }

    const float mq2Voltage = mq2ReadVoltage();
    const float sharpVoltage = sharpReadVoltage();

    Serial.printf("MQ2: %.3f V\n", mq2Voltage);
    Serial.printf("Sharp: %.3f V\n", sharpVoltage);

    if (wifiConnected) {
      uploadSensorReadings(
        bmp180IsAvailable(),
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
