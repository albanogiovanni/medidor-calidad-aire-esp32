#include "FirebaseService.h"
#include <Arduino.h>
#include <FirebaseESP32.h>
#include "keys.h"

static FirebaseData fbdo;
static FirebaseAuth auth;
static FirebaseConfig config;

void setupFirebase(void) {
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void uploadSensorReadings(
  bool bmp180Available,
  float temperatureC,
  float pressurePa,
  float mq2Voltage,
  float sharpVoltage
) {
  if (!Firebase.ready()) {
    Serial.println("Firebase no disponible. Dato no enviado.");
    return;
  }

  FirebaseJson json;
  json.set("bmp180_ok", bmp180Available);
  if (bmp180Available) {
    json.set("temperatura_c", temperatureC);
    json.set("presion_pa", pressurePa);
  }

  json.set("mq2_voltaje", mq2Voltage);
  json.set("sharp_voltaje", sharpVoltage);
  json.set("timestamp/.sv", "timestamp");

  if (Firebase.pushJSON(fbdo, "/historial_sensores", json)) {
    Serial.println("Dato guardado con marca de tiempo.");
  } else {
    Serial.println(fbdo.errorReason());
  }
}
