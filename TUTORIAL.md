# Instructions

## Offline
### Device Assembly
First, you need to buy all the necessary components:
* ESP32-WROOM-32 devkit v1 (Microcontroller)
* BME280 (Measures temperature, humidity, pressure)
* SCD-40 (CO2 sensor)
* OLED screen ssd1306 128x64p 

<span style="color:red">**PLEASE NOTE! ALL COMPONENTS MUST USE THE I2C BUS!**</span>

When purchasing, pay attention to what the BME280 looks like, because it is often confused with the BMP280 due to their similarity!
<img src="img/BMPvsBME.jpeg" alt="bme and bmp" width="50%" />

Here is what all the components look like (with soldered pins):

*Photo of the components*

Next, you need to connect all the modules together into a single device.
![Connection diagram](img/connectionScheme.png)

1. 3.3v(esp32) -> VIN(bme280) -> VDD(SCD-40, might be labeled as VCC) -> VCC(ssd1306)
2. GND(esp32) -> GND(bme280) -> GND(SCD-40) -> GND(ssd1306)
3. D21(esp32) -> SDA(bme280) -> SDA(SCD-40) -> SDA(ssd1306)
4. D22(esp32) -> SCL(bme280) -> SCL(SCD-40) -> SCL(ssd1306)
