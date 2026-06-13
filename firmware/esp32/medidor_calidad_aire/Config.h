#pragma once

// Pines I2C (BMP180)
static const int BMP180_SDA_PIN = 21;
static const int BMP180_SCL_PIN = 22;

// Pines ADC
static const int MQ2_ADC_PIN = 34;
static const int SHARP_ADC_PIN = 35;

// Pines digitales
static const int SHARP_LED_PIN = 25;
static const int CONFIG_BUTTON_PIN = 0; // GPIO0 = boton BOOT

// Niveles LED Sharp (activo en bajo)
static const int SHARP_LED_ON_LEVEL = LOW;
static const int SHARP_LED_OFF_LEVEL = HIGH;

// Muestras por lectura
static const int MQ2_SAMPLE_COUNT = 8;
static const int SHARP_SAMPLE_COUNT = 4;

// Tiempos
static const unsigned long LOOP_DELAY_MS = 10000;
static const unsigned long WIFI_RETRY_MS = 15000;
