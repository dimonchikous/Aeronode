#ifndef AERONODE_SENSORS_H
#define AERONODE_SENSORS_H

#include <Arduino.h>
#include <AeronodeConfig.h>

//BME280 functions
int getTemp(void);
int getHum(void);

//SCD-40 functions
int getCO2(void);

AirQuality getAllMetrics(void);

#endif
