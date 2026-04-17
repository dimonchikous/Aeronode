#include "AeroSensors.h"
#include <Arduino.h>		//Arduino
#include <Wire.h>		//I2C
#include <SensirionI2cScd4x.h>	//SCD-40
#include <Adafruit_BME280.h>	//BME280
#include <Adafruit_Sensor.h>	//Sensors lib
#include <AeronodeConfig.h> 	//Config

int getTemp(void){
	float fTemp = bme.readTemperature();

	static float lastValidTemp=0;

	if (isnan(fTemp))
		return (int)lastValidTemp;
	lastValidTemp = round(fTemp);
	return (int)lastValidTemp;
}

int getHum(void){
	float fHum = bme.readHumidity();

	static float lastValidHum = 0;

	if (isnan(fHum))
		return (int)lastValidHum;
	lastValidHum = round(fHum);
	return (int)lastValidHum;
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
			return lastValidCO2;
		}
	}
	return lastValidCO2;
}

AirQuality getAllMetrics(void){
	AirQuality data;

	data.temp = getTemp();
	data.hum = getHum();
	data.co2 = getCO2();

	return data;
}
