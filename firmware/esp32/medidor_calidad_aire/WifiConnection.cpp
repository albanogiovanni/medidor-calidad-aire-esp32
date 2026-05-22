#include "WifiConnection.h"
#include <Arduino.h>
#include <WiFi.h>
#include "keys.h"

static const unsigned long WIFI_TIMEOUT_MS = 15000;

bool connectWiFi(void) {
  Serial.print("Conectando a WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    return true;
  }

  Serial.println("\nNo se pudo conectar a WiFi. El sistema seguira leyendo sensores.");
  return false;
}
