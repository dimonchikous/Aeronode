#ifndef AERONODECONFIG
#define AERONODECONFIG

#include <Arduino.h>			//Arduino
#include <Wire.h>			//I2C
#include <SensirionI2cScd4x.h>		//SCD-40
#include <Adafruit_GFX.h> 		//Graphics
#include <Adafruit_SSD1306.h>		//SSD1306
#include <Adafruit_BME280.h>		//BME280
#include <Adafruit_Sensor.h>		//Sensors lib

//Online
#define AERONODE_ONLINE 0

#if AERONODE_ONLINE == 1
	#define DEFAULT_PASSWORD "12345678"	//Device default password
	#define POST_INTERVAL 60000		//Json package posting interval
	
	#include <WiFi.h>
	#include <WiFiManager.h>
	#include <Preferences.h>
	#include <HTTPClient.h>
	#include <ArduinoJson.h>
#endif

//I2C adresses
#define I2C_OLED 0x3c
#define I2C_SCD 0x62
#define I2C_BME 0x76

//I2C pins
#define I2C_AERONODE_SDA 21
#define I2C_AERONODE_SCL 22

//OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//Objects
extern Adafruit_SSD1306 display;
extern SensirionI2cScd4x scd4x;
extern Adafruit_BME280 bme;

struct AirQuality {
	int temp;
	int hum;
	int co2;
};

#endif
