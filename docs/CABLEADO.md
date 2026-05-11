## 1. SHT30

Conexion

| SHT30 | ESP32  |
|-------|--------|
| VIN   | 5 V    |
| GND   | GND    |
| SDA   | GPIO21 |
| SCL   | GPIO22 |

Notas importantes:
- En revision!!!
- Antes de conectar `VIN` al `ESP32`, verifica con multimetro.

## 2. MQ-2

| MQ-2 | ESP32  |
|------|--------|
| VCC  | 5V     |
| GND  | GND    |
| AOUT | GPIO34 |

Notas importantes:
- El `MQ-2` necesita tiempo de calentamiento para lecturas mas estables.

## 3. Sharp GP2Y1014AU0F

Conexion logica usada en el firmware:

| Sharp | ESP32 |
|-------|-------|
| VLED/LED control del circuito | GPIO25 |
| Vo | GPIO35 |
| VCC | segun tu circuito |
| GND | GND |

Notas importantes:
- No conectes el LED interno del sensor directamente al GPIO del `ESP32`.
- Usa el circuito de manejo recomendado por el datasheet, normalmente con transistor, resistencia.
- El codigo asume que el control del LED es activo en bajo. Si tu circuito responde al reves, cambia `SHARP_LED_ON_LEVEL` y `SHARP_LED_OFF_LEVEL`.
- Verifica que `Vo` no supere `3.3V` al entrar al `ESP32`.