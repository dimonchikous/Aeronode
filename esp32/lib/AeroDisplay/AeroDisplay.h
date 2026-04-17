#ifndef AERONODE_DISPLAY_H
#define AERONODE_DISPLAY_H

#include <Arduino.h>
#include <AeronodeConfig.h>

//Bitmap icons

//Temperature (8x8)
const unsigned char temIco[] PROGMEM = {
  0x04, 0x0A, 0x0A, 0x0A, 0x0E, 0x1F, 0x1F, 0x0E
};

//Humidity (8x8)
const unsigned char humIco[] PROGMEM = {
  0x04, 0x04, 0x0E, 0x0E, 0x1F, 0x1F, 0x1F, 0x0E
};

//CO2 (8x8)
const unsigned char co2Ico[] PROGMEM = {
  0x00, 0x06, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00
};

//Oled functions
void displayMetrics(struct AirQuality currentData);

#endif
