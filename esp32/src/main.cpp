#include <Arduino.h>			//Arduino
#include <Wire.h>			//I2C
#include <SensirionI2cScd4x.h>		//SCD-40
#include <Adafruit_GFX.h> 		//Graphics
#include <Adafruit_SSD1306.h>		//SSD1306 

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define I2C_OLED 0x3c
#define I2C_SCD 0x62

//bitmap icons

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

struct AirQuality {
	int temp;
	int hum;
	int co2;
};

//BME280 functions
int getTemp(void);
int getHum(void);

//SCD-40 functions
int getCO2(void);

//Oled functions
void displayMetrics(struct AirQuality currentData); 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
SensirionI2cScd4x scd4x;

void setup(void){
	Serial.begin(115200);
	//I2C part
	Wire.begin(21, 22);

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

	Serial.println("Setup complete successfully!");
}


void loop(void){
	static unsigned long lastTime = 0;
	const unsigned long interval = 5000;	
	
	if (millis()-lastTime>=interval){
		lastTime=millis();

		struct AirQuality data;
	
		data.temp = getTemp();
		data.hum = getHum();
		data.co2 = getCO2();

		displayMetrics(data);
	}
}

int getTemp(void){
	//Waiting for bme280
	return 24;
}

int getHum(void){
	//waiting for bme280
	return 100;
}

int getCO2(void){
	static int lastValidCO2 = 0;

	uint16_t co2 = 0;
	float trTemp = 0.0f;
	float trHum = 0.0f;

	bool isDataReady = false;
	scd4x.getDataReadyStatus(isDataReady);

	if (isDataReady){
		uint16_t scdError = scd4x.readMeasurement(co2, trTemp, trHum);
		if (!scdError){
			lastValidCO2=(int)co2;
			return lastValidCO2;
		} else {
			Serial.print("Error while reading SCD-40: ");
			Serial.println(scdError);
			return -1;
		}
	}
	return lastValidCO2;
}

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
