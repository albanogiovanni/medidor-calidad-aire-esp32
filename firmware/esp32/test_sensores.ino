#include <Wire.h>
#include <Adafruit_BMP085.h>

/*
 * Prueba para ESP32 con tres sensores:
 * 1. BMP180 por I2C (temperatura + presión)
 * 2. MQ-2 por ADC
 * 3. Sharp GP2Y1014AU0F por GPIO + ADC
 */

/* Pin bus I2C usado por el BMP180. */
static const int BMP180_SDA_PIN = 21;
static const int BMP180_SCL_PIN = 22;

/* Objeto sensor BMP180 */
static Adafruit_BMP085 bmp;

/* Pin ADC donde se conecta la salida analogica AOUT del MQ-2. */
static const int MQ2_ADC_PIN = 34;

/* Pin ADC donde se conecta la salida analogica Vo del sensor Sharp. */
static const int SHARP_ADC_PIN = 35;
static const int SHARP_LED_PIN = 25;
static const int SHARP_LED_ON_LEVEL = LOW;
static const int SHARP_LED_OFF_LEVEL = HIGH;


static const float ADC_MAX_READING = 4095.0f;
static const float ADC_VREF_ESTIMATE = 3.3f;

/*
 * Convierte una lectura ADC cruda a un voltaje aproximado.
 * Sirve solo para tener una referencia visual rapida en terminal.
 */
static float adcRawToVoltage(int raw)
{
  /* Divide el valor crudo entre el maximo ADC y lo escala a 3.3V. */
  return (raw / ADC_MAX_READING) * ADC_VREF_ESTIMATE;
}

/*
 * Lee una medicion del BMP180.
 *
 * Parametros de salida:
 * - temperatureC: temperatura en grados Celsius.
 * - pressurePa: presion atmosferica en Pascales.
 *
 * Devuelve:
 * - true si el sensor respondio correctamente.
 * - false si fallo la comunicacion I2C.
 */
static bool readBmp180(float *temperatureC, float *pressurePa)
{
  *temperatureC = bmp.readTemperature();
  *pressurePa = bmp.readPressure();

  if (*temperatureC <= 0 && *pressurePa <= 0) {
    return false;
  }

  return true;
}

/*
 * Lee el MQ-2 varias veces y devuelve un promedio simple.
 * Esto ayuda a que la lectura se vea menos ruidosa en terminal.
 */
static int readMq2Raw(void)
{
  int total = 0;
  for (int i = 0; i < 8; i++) {
    total += analogRead(MQ2_ADC_PIN);
    delayMicroseconds(200);
  }
  return total / 8;
}

/*
 * Lee el sensor Sharp usando una secuencia minima de temporizacion.
 * La idea es encender el LED IR, esperar un tiempo corto y leer el ADC
 * cerca del instante util sugerido para este tipo de sensor.
 */
static int readSharpRaw(void)
{
  int raw = 0;
  digitalWrite(SHARP_LED_PIN, SHARP_LED_ON_LEVEL);
  delayMicroseconds(280);
  raw = analogRead(SHARP_ADC_PIN);

  delayMicroseconds(40);


  digitalWrite(SHARP_LED_PIN, SHARP_LED_OFF_LEVEL);

  delayMicroseconds(9680);

  return raw;
}


void setup()
{
  Serial.begin(115200);

  delay(1000);

  Wire.begin(BMP180_SDA_PIN, BMP180_SCL_PIN);

  if (!bmp.begin()) {
    Serial.println("BMP180 no encontrado! Verificar conexiones.");
    while (1) delay(1000);
  }

  pinMode(SHARP_LED_PIN, OUTPUT);

  digitalWrite(SHARP_LED_PIN, SHARP_LED_OFF_LEVEL);

  analogReadResolution(12);

  Serial.println("Iniciando prueba de sensores");
  Serial.print("I2C OK en SDA=");
  Serial.print(BMP180_SDA_PIN);
  Serial.print(" SCL=");
  Serial.println(BMP180_SCL_PIN);
  Serial.println("ADC OK para MQ-2 y Sharp");
  Serial.print("GPIO Sharp OK en GPIO=");
  Serial.println(SHARP_LED_PIN);
  Serial.println();
}


void loop()
{
  float temperatureC = 0.0f;
  float pressurePa = 0.0f;

  const bool bmp180_is_ok = readBmp180(&temperatureC, &pressurePa);

  const int mq2Raw = readMq2Raw();
  const int sharpRaw = readSharpRaw();
  if (bmp180_is_ok) {
    Serial.print("BMP180 -> Temp: ");
    Serial.print(temperatureC, 2);
    Serial.print(" C | Presion: ");
    Serial.print(pressurePa, 0);
    Serial.println(" Pa");
  } else {
    Serial.println("BMP180 sin respuesta");
  }

  /* Muestra el valor crudo del MQ-2 y un voltaje aproximado para referencia. */
  Serial.print("MQ-2  -> ADC: ");
  Serial.print(mq2Raw);
  Serial.print(" | Volt aprox: ");
  Serial.print(adcRawToVoltage(mq2Raw), 2);
  Serial.println(" V");

  /* Muestra el valor crudo del Sharp y un voltaje aproximado para referencia. */
  Serial.print("SHARP -> ADC: ");
  Serial.print(sharpRaw);
  Serial.print(" | Volt aprox: ");
  Serial.print(adcRawToVoltage(sharpRaw), 2);
  Serial.println(" V");

  Serial.println();

  delay(5000);
}
