# Medidor de Calidad de Aire

Repositorio academico del proyecto `Medidor de Calidad de Aire`, desarrollado en el marco de la materia `Laboratorio de Sistemas Embebidos`. El trabajo propone un dispositivo embebido de bajo costo para el monitoreo indicativo de contaminantes en zonas cercanas al vertedero municipal de San Carlos de Bariloche.

## Importante

Este proyecto:
- Solo tiene fines academicos
- se encuentra actualmente en desarrollo
- no representa un producto final ni una solucion lista para produccion

El sistema se plantea como una herramienta de monitoreo indicativo y concientizacion, no como un instrumento oficial de fiscalizacion ambiental.

## Descripcion General

El sistema consiste en una unidad de adquisicion de datos ambientales basada en `ESP32`, capaz de medir material particulado, gases combustibles o humo, temperatura y presion. La propuesta contempla almacenamiento local, visualizacion simple para el usuario y respaldo opcional de registros.

## Alcance

El repositorio contempla:
- documentacion academica del proyecto
- pruebas iniciales de lectura de sensores
- guia tecnica de cableado de referencia
- organizacion del material para seguimiento del desarrollo

## Tecnologias y Componentes Considerados

- `ESP32`
- `Arduino Framework en C++`
- sensor `SHT30`
- sensor `MQ-2`
- sensor `Sharp GP2Y1014AU0F`

## Estructura del Repositorio

```text
.
├── README.md
├── .gitignore
├── firmware/
│   └── esp32/
│       └── sensores_test/
│           └── sensores_test.ino
├── docs/
│   ├── informes/
│   │   ├── 01_Informe_de_Viabilidad.pdf
│   │   ├── 02_Informe_de_Requerimientos.pdf
│   │   └── 03_Informe_de_Planificacion.pdf
│   └── CABLEADO.md
└── assets/
    └── img/
```

## Descripcion de Carpetas

- `firmware/`: codigo y pruebas de firmware del proyecto
- `firmware/esp32/`: implementaciones y pruebas dirigidas a la plataforma `ESP32`
- `docs/`: documentacion general del proyecto
- `docs/informes/`: entregables e informes academicos
- `assets/img/`: imagenes, diagramas o recursos visuales de apoyo

## Documentacion Disponible

- `docs/informes/01_Informe_de_Viabilidad.pdf`
- `docs/informes/02_Informe_de_Requerimientos.pdf`
- `docs/informes/03_Informe_de_Planificacion.pdf`
- `docs/CABLEADO.md`

## Estado del Proyecto

Proyecto en desarrollo.

Actualmente se dispone de:
- documentacion base del proyecto
- pruebas preliminares de lectura de sensores en `ESP32`
- referencia de conexiones para los sensores considerados

Segun la planificacion actual, el trabajo contempla etapas de desarrollo de firmware, pruebas iniciales con sensores, desarrollo de aplicacion y ajustes finales del prototipo.

## Pendientes por Definir

- frecuencia de muestreo de cada sensor
- umbrales para categorias `normal`, `precaucion` y `alerta`
- autonomia esperada de la bateria
- tecnologia de comunicacion con la aplicacion
- formato final de almacenamiento local
- material y geometria final de la carcasa
- procedimiento de verificacion o calibracion de sensores

## Nota Academica

Este repositorio funciona como respaldo tecnico y documental de un proyecto universitario en desarrollo. Su contenido puede cambiar conforme avance la planificacion, el analisis y las pruebas del curso.
