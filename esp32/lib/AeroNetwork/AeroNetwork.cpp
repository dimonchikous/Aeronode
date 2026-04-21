#include "AeroNetwork.h"

#if AERONODE_ONLINE == 1
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <Preferences.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

Preferences pref;
String serverIp;
String deviceName;
bool saveNeeded = false;

void saveCallback(){
	saveNeeded = true;
}

void initAeroNetwork(void){
	pref.begin("aeronode", false);
	serverIp = pref.getString("server_ip", "192.168.1.100:5000");

	String devPrefix = pref.getString("dev_prefix", "Aeronode");

	uint8_t mac[6];
	WiFi.macAddress(mac);
	char macSuffix[10];
	sprintf(macSuffix, "-%02X:%02X", mac[4], mac[5]);

	deviceName=devPrefix+String(macSuffix);

	//WiFiManager config
	WiFiManager wm;
	wm.setSaveConfigCallback(saveCallback);

	//Portal
	WiFiManagerParameter customServerIp("server", "Server IP(with port)", serverIp.c_str(), 40);
	WiFiManagerParameter customDevPrefix("prefix", "DeviceName(Without MAC)", devPrefix.c_str(), 20);
	wm.addParameter(&customServerIp);
	wm.addParameter(&customDevPrefix);

	//Reset button
	pinMode(0, INPUT_PULLUP);
	if (digitalRead(0) == LOW){
		Serial.println("INFO: Button is pressed on start! Starting portal.");

		wm.setConfigPortalTimeout(300);
		if (!wm.startConfigPortal(deviceName.c_str(), DEFAULT_PASSWORD)){
			Serial.println("INFO: TIMEOUT. Reboot...");
			ESP.restart();
		}
	}else{
		Serial.println("INFO: Connection...");

		if(!wm.autoConnect(deviceName.c_str(), DEFAULT_PASSWORD)){
			Serial.println("ERROR: NO CONNECTION. Reboot...");
			ESP.restart();
		}
	}

	Serial.println("INFO: WiFi connected!");

	if (saveNeeded){
		serverIp = customServerIp.getValue();
		devPrefix = customDevPrefix.getValue();

		pref.putString("server_ip", serverIp);
		pref.putString("dev_prefix", devPrefix);

		deviceName = devPrefix+String(macSuffix);

		Serial.println("INFO: Config saved!");
	}
	pref.end();
}

void sendMetrics(AirQuality data){
	if(WiFi.status() == WL_CONNECTED){
		HTTPClient http;

		String url="http://"+serverIp+"/update";
		http.begin(url);

		http.addHeader("Content-Type", "application/json");
		
		//forming json document
		JsonDocument doc;
		doc["name"] = deviceName;
		doc["temp"] = data.temp;
		doc["hum"] = data.hum;
		doc["co2"] = data.co2;

		String requestBody;
		serializeJson(doc, requestBody);

		unsigned long startTime = millis();
		int httpResponseCode = http.POST(requestBody);
		unsigned long duration = millis()-startTime;

		if(httpResponseCode>0){
			Serial.printf("[HTTP]INFO: POST Succes: %d (took %lums)\n", httpResponseCode, duration);
		}else{
			Serial.printf("[HTTP]ERROR: POST Failed: %s\n", http.errorToString(httpResponseCode).c_str());
		}
		http.end();
	}else{
		Serial.println("[NETWORK]ERROR: WiFi disconnected, skipping send");
	}
}
#endif
