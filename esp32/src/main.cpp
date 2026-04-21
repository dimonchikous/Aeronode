#include <Arduino.h>			//Arduino
#include <Wire.h>			//I2C
#include <SensirionI2cScd4x.h>		//SCD-40
#include <Adafruit_GFX.h> 		//Graphics
#include <Adafruit_SSD1306.h>		//SSD1306 
#include <Adafruit_BME280.h>		//BME280
#include <Adafruit_Sensor.h>		//Sensors lib
#include <WiFi.h>			//WiFi lib
#include <WiFiManager.h>		//WiFi manager
#include <Preferences.h>		//Net preferences
#include <HTTPClient.h>			//HTTP client
#include <ArduinoJson.h>		//Json
#include <AeronodeConfig.h>		//Config
#include <AeroSensors.h>		//Sensors
#include <AeroDisplay.h>		//Display and bitmaps
#include <AeroNetwork.h>		//Network work

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
SensirionI2cScd4x scd4x;
Adafruit_BME280 bme;

void setup(void){
	Serial.begin(115200);
	//I2C part
	Wire.begin(I2C_AERONODE_SDA, I2C_AERONODE_SCL);

	//OLED part
	if (!display.begin(SSD1306_SWITCHCAPVCC, I2C_OLED)){
		Serial.println("ERROR: OLED NOT FOUND!");
		while(1)
			delay(1);
	}
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(SSD1306_WHITE);
	display.setCursor(0,0);
	display.println("Aeronode is starting...");
	display.display();

	//scd-40 part
	scd4x.begin(Wire, I2C_SCD);
	scd4x.stopPeriodicMeasurement();

	uint16_t scdError = scd4x.startPeriodicMeasurement(); 
	if (scdError){
		Serial.print("ERROR: SCD-40 NOT FOUND! Code: ");
		Serial.println(scdError);

		display.clearDisplay();
		display.setCursor(0,0);
		display.print("No scd-40");
		display.display();
		while(1)
			delay(1);
	}

	if (!bme.begin(I2C_BME)){
		Serial.println("ERROR: BME280 NOT FOUND!");

		display.clearDisplay();
		display.setCursor(0,0);
		display.print("No BME280");
		display.display();
		while(1)
			delay(1);
	}

	#if AERONODE_ONLINE == 1
		display.clearDisplay();
		display.setCursor(0,0);
		display.println("Starting WiFi...");
		display.display();

		initAeroNetwork();
	#endif

	Serial.println("Setup complete successfully!");
}


void loop(void){
	//display timer config
	static unsigned long lastDisplayTime = 0;
	const unsigned long displayInterval = 5000;	
	
	//network timer config
	static unsigned long lastNetworkTime = 0;
	const unsigned long networkInterval = 60000;
	
	if (millis()-lastDisplayTime>=displayInterval){
		lastDisplayTime=millis();

		struct AirQuality data = getAllMetrics();

		displayMetrics(data);

		}
	#if AERONODE_ONLINE == 1
	if (millis() - lastNetworkTime >= networkInterval){
		lastNetworkTime = millis();
		struct AirQuality data = getAllMetrics();
		sendMetrics(data);
	}
	#endif

}
