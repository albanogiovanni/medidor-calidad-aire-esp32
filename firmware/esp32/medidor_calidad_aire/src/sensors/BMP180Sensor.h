#pragma once

void bmp180Begin();
bool bmp180IsAvailable();
void bmp180Read(float *temperatureC, float *pressurePa);
