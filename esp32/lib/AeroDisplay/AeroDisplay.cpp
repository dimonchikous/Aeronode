#include <Arduino.h>			//Arduino
#include <Wire.h>			//I2C
#include <Adafruit_GFX.h> 		//Graphics
#include <Adafruit_SSD1306.h>		//SSD1306
#include <AeronodeConfig.h>		//Config
#include "AeroDisplay.h"

void displayMetrics(struct AirQuality currentData){
	display.clearDisplay();

	display.setTextColor(SSD1306_WHITE);

	display.setTextSize(1);
	display.setCursor(40,0);
	display.println("AERONODE");
	display.drawFastHLine(0, 10, 128, SSD1306_WHITE);

	//temperature
	display.drawBitmap(10, 18, temIco, 8, 8, SSD1306_WHITE);
	display.setCursor(30, 18);
	display.printf("Temp: %d%cC", currentData.temp, (char)247);

	//humidity
	display.drawBitmap(10, 34, humIco, 8, 8, SSD1306_WHITE);
	display.setCursor(30, 34);
	display.printf("Hum: %d%%", currentData.hum);

	//co2
	display.drawBitmap(10, 50, co2Ico, 8, 8, SSD1306_WHITE);
	display.setCursor(30, 50);
	display.printf("CO2: %dppm", currentData.co2);

	display.display();
}
