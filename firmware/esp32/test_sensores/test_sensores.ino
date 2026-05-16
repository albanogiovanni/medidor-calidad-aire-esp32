#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "keys.h"

// Objetos de Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

/* Pines y sensores originales */
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

/* Funciones de lectura originales */
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

  // Conexión Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado!");

  // Configuración Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  /* BMP180 */
  Wire.begin(BMP180_SDA_PIN, BMP180_SCL_PIN);
  bmp180Available = bmp.begin();

  if (!bmp180Available) {
    Serial.println("BMP180 no encontrado!");
  }

  /* SHARP */
  pinMode(SHARP_LED_PIN, OUTPUT);
  digitalWrite(SHARP_LED_PIN, SHARP_LED_OFF_LEVEL);

  /* ADC */
  analogReadResolution(12);
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

  // Mostrar en Serial
  Serial.printf("T: %.2f C | P: %.2f Pa | MQ2: %d | Sharp: %d\n", 
                temperatureC, pressurePa, mq2Raw, sharpRaw);

  if (Firebase.ready()) {
    FirebaseJson json;
    json.set("temperatura", temperatureC);
    json.set("presion", pressurePa);
    json.set("mq2", mq2Raw);
    json.set("sharp", sharpRaw);
    
    // ESTO AGREGA LA FECHA Y HORA DEL SERVIDOR (Server Value Timestamp)
    // Se guarda como milisegundos desde 1970 (Unix Epoch)
    json.set("timestamp/.sv", "timestamp"); 

    if (Firebase.pushJSON(fbdo, "/historial_sensores", json)) {
      Serial.println("Dato guardado con marca de tiempo.");
    } else {
      Serial.println(fbdo.errorReason());
    }
  }

  delay(LOOP_DELAY_MS);
}
