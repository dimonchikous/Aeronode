# Instructions

## Offline
### Device Assembly
First, you need to purchase all the necessary components:
* **ESP32-WROOM-32 DevKit V1** (Microcontroller)
* **BME280** (Measures temperature, humidity, and pressure)
* **SCD-40** (CO2 sensor)
* **SSD1306 OLED Display** (128x64 pixels, white color)

<span style="color:red">**PLEASE NOTE! ALL COMPONENTS MUST SUPPORT THE I2C BUS!**</span>

When purchasing, pay close attention to the visual differences between the BME280 and the BMP280, as they are often confused due to their identical appearance!
<img src="img/BMPvsBME.jpeg" alt="bme and bmp" width="50%" />

Next, connect all the modules together into a single device according to the scheme:
![Connection diagram](img/connectionScheme.png)

1. **3.3V (ESP32)** -> **VIN** (BME280) -> **VDD** (SCD-40, may be labeled VCC) -> **VCC** (SSD1306)
2. **GND (ESP32)** -> **GND** (BME280) -> **GND** (SCD-40) -> **GND** (SSD1306)
3. **D21 (ESP32)** -> **SDA** (BME280) -> **SDA** (SCD-40) -> **SDA** (SSD1306)
4. **D22 (ESP32)** -> **SCL** (BME280) -> **SCL** (SCD-40) -> **SCL** (SSD1306)

---

### Firmware Flashing

#### Method 1: PlatformIO
1. Clone the repository:
```bash
git clone https://github.com/dimonchikous/Aeronode.git
```
2. Navigate to the project directory: `Aeronode/esp32/`
3. (Optional) Configure settings in `lib/AeronodeConfig/AeronodeConfig.h` and `lib/AeroDisplay/AeroDisplay.h`
4. Connect the ESP32 to your computer via USB.
5. Build and upload the code:
```bash
pio run -t upload
```

#### Method 2: esptool
1. Install **Python**.
2. Install the flashing utility:
```bash
pip install esptool
```
3. Connect the ESP32 to your computer and identify the port.

**On Linux:**
```bash
ls /dev/ttyUSB*
# or
ls /dev/ttyACM*
```
Commonly it's `/dev/ttyUSB0`.

**On Windows:**
* Open **Device Manager**.
* Check the **"Ports (COM & LPT)"** section. Usually it's `COM3` or `COM4`.

4. Download the `Aeronode.bin` file from the **Releases** section.
5. Flash the device:
```bash
esptool --chip esp32 --port <YourPort> --baud 921600 write_flash 0x0 Aeronode.bin
```
*(Replace `<YourPort>` with your actual port name)*.

#### Method 3: Adafruit WebSerial 
1. Connect the ESP32 via USB. Ensure no other programs are using the serial port.
2. Download the `Aeronode.bin` file from the **Releases** section.
3. Open [Adafruit WebSerial ESPTool](https://adafruit.github.io/Adafruit_WebSerial_ESPTool/).
4. Click **Connect** and select your board (e.g., "USB Serial").
5. Click the **"Choose a file"** button and select `Aeronode.bin`.
    * **Crucial:** Ensure the **Offset** is set to **`0x0`**.
6. Click **Program** and wait for the process to complete. Once finished, press the **Reset** button on the ESP32.

---

#### Additional Info
* If you encounter issues with the methods above, please report them in the **Issues** or try an alternative flashing method.
