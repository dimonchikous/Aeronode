# Instructions

## Offline Mode
### Device Assembly
First, you need to purchase all the necessary components:
* **ESP32-WROOM-32 DevKit V1** (Microcontroller)
* **BME280** (Measures temperature, humidity, and atmospheric pressure)
* **SCD-40** (CO2 sensor)
* **SSD1306 OLED Display** 128x64p (White color)

<span style="color:red">**PLEASE NOTE! ALL COMPONENTS MUST SUPPORT THE I2C BUS!**</span>

When purchasing, pay close attention to the visual differences between the BME280 and the BMP280, as they are often confused due to their identical appearance!
<img src="img/BMPvsBME.jpeg" alt="bme and bmp" width="50%" />

Next, connect all the modules together into a single device according to the wiring diagram:
![Connection diagram](img/connectionScheme.png)

1. **3.3V (ESP32)** -> **VIN** (BME280) -> **VDD** (SCD-40, might be labeled as VCC) -> **VCC** (SSD1306)
2. **GND (ESP32)** -> **GND** (BME280) -> **GND** (SCD-40) -> **GND** (SSD1306)
3. **D21 (ESP32)** -> **SDA** (BME280) -> **SDA** (SCD-40) -> **SDA** (SSD1306)
4. **D22 (ESP32)** -> **SCL** (BME280) -> **SCL** (SCD-40) -> **SCL** (SSD1306)

---

### Flashing the Firmware

#### Method 1: PlatformIO (Recommended for developers)
1. Clone the repository:
```bash
git clone https://github.com/dimonchikous/Aeronode.git
```
2. Navigate to the project directory: `Aeronode/esp32/`
3. *(Optional)* Configure the settings in `lib/AeronodeConfig/AeronodeConfig.h` and `lib/AeroDisplay/AeroDisplay.h`.
4. Connect the ESP32 to your computer via USB.
5. Build and upload the code:
```bash
pio run -t upload
```

#### Method 2: esptool (CLI)
1. Install **Python**.
2. Install the flashing utility:
```bash
pip install esptool
```
3. Connect the ESP32 to your computer and identify the port.

**On Linux:** 
```bash
ls /dev/ttyUSB*
# or if not found:
ls /dev/ttyACM*
```
Commonly it's `/dev/ttyUSB0`.

**On Windows:**
* Open **Device Manager**.
* Check the **"Ports (COM & LPT)"** section. Usually, it's `COM3`, `COM4`, etc.

4. Download the `Aeronode-offline.bin` file from the **Releases** section.
5. Flash the device:
```bash
esptool --chip esp32 --port <YourPort> --baud 921600 write_flash 0x0 Aeronode-offline.bin
```
*(Replace `<YourPort>` with your actual port name)*.

#### Method 3: Adafruit WebSerial (Browser-based)
1. Connect the ESP32 to your computer. Ensure no other programs are actively using the serial port.
2. Download the `Aeronode-offline.bin` file from the **Releases** section.
3. Open [Adafruit WebSerial ESPTool](https://adafruit.github.io/Adafruit_WebSerial_ESPTool/). 
4. Click **Connect** and select your board in the pop-up window (e.g., "USB Serial").
5. Click the **"Choose a file"** button and select `Aeronode-offline.bin`.
    * **Crucial:** Ensure the **Offset** is set to **`0x0`**.
6. Click **Program** and wait for the upload to complete. Once finished, press the **Reset** button on the ESP32 or reconnect the USB cable.

---

#### Additional Information
* If the methods above did not work for you, please create a ticket in the **Issues** section or try other available flashing tools.


## Online Mode
### ESP32 Configuration
1. Open the `AeronodeConfig.h` file in any text editor:
    * `esp32/lib/AeronodeConfig/AeronodeConfig.h`
2. Enable (set to 1) the **AERONODE_ONLINE** parameter:
    * `#define AERONODE_ONLINE 1` 
3. Save the file.
4. Upload the firmware (as described above).
    * If using the **.bin** method, download and flash the file named `Aeronode-online.bin`.
5. Upon powering the device for the first time, connect to the Wi-Fi Access Point it creates to configure your network settings.
    * **Please note!** During the setup process (when specifying the server's IP address), you will need to provide a **port**. The default bot port is **5000**. To configure a custom port, change the `port` value at the very end of the `server/main.py` file to your desired one.

### Bot Configuration
**Important**: The instructions below are written primarily for Linux systems.

1. Install the required packages:
    * `python3-venv` 
    * `python3-pip`
    * `ufw`
    * `rsync`
2. Configure **ufw** (Firewall):
```bash
sudo ufw allow 5000/tcp
sudo ufw allow 22/tcp
sudo ufw enable
```
3. Create a dedicated user for the project and log in as that user.
4. Clone the project into the **home** directory and navigate to the project folder.
5. Set up the virtual environment:
```bash
python3 -m venv venv
source venv/bin/activate # or activate.fish depending on your shell
pip install -r requirements.txt
```
6. .env Configuration
Open the **.env** file and configure it.

**BOT_TOKEN** - The Telegram bot token obtained from @BotFather.

**USE_WHITELIST** - Flag to enable the node whitelist. Default is False.

**USE_WEATHER** - Flag to enable outdoor weather monitoring. Default is True.

**USE_CRITICAL_CO2_DETECTOR** - Flag to enable high CO2 level notifications. Default is True.

**USE_TIME_NOTIFICATION** - Flag to enable the daily alarm/digest that sends air quality information at a specific time. Default is True.

**USE_SLEEPTIME** - *(Requires USE_CRITICAL_CO2_DETECTOR and USE_TIME_NOTIFICATION flags)!* Flag to mute high CO2 notifications from a specified time until the daily alarm time (Do Not Disturb mode). Default is True.

**WEATHER_CITY** - The name or GPS coordinates of the location where you want to check the weather (if USE_WEATHER is enabled). If not specified, the city will be determined automatically based on your IP address.

**MAX_CRITICAL_VALUE** - *(Requires the USE_CRITICAL_CO2_DETECTOR flag)!* The CO2 level at which a high CO2 warning notification will be sent. Default is 1100.

**NORMAL_CRITICAL_VALUE** - *(Requires the USE_CRITICAL_CO2_DETECTOR flag)!* The CO2 level that is considered normal. Default is 800.

**NOTIFICATION_TIME** - *(Requires the USE_TIME_NOTIFICATION flag)!* The time when the daily air quality information will be sent. Specified in HH:MM format *(H - hour, M - minute)*. Default is 06:00.

**NOTIFICATION_MSG_START** - *(Requires the USE_TIME_NOTIFICATION flag)!* The opening sentence of the daily notification. HTML tags are allowed. Default is "Hello!".

**SLEEPTIME** - *(Requires the USE_SLEEPTIME flag)!* The time from which high CO2 notifications are muted until the daily alarm time. Specified in HH:MM format *(H - hour, M - minute)*. Default is 00:00.

**Example**
```env
BOT_TOKEN=YOUR_BOT_TOKEN

USE_WHITELIST=False
USE_WEATHER=True
USE_CRITICAL_CO2_DETECTOR=True
USE_TIME_NOTIFICATION=True
USE_SLEEPTIME=True

WEATHER_CITY=Moscow

MAX_CRITICAL_VALUE=1100
NORMAL_CRITICAL_VALUE=800

NOTIFICATION_TIME=06:00
NOTIFICATION_MSG_START=Hello!

SLEEPTIME=00:00
```
7. Configure `users.txt`:
Open the `users.txt` file.

Enter the Telegram user IDs that are allowed to use your bot (one per line). For example:
```text
000000000
```
8. Adding nodes to the blacklist:
Open the `blacklist.txt` file.

Enter the last 2 bytes of the MAC address of the nodes you want to block. For example:
```text
A2:C6
C2:B2
```
9. Whitelist mode:
In the `.env` file, set the **USE_WHITELIST** flag to **True**.

Open the `whitelist.txt` file and specify the last 2 bytes of the MAC address of the nodes that should be allowed. For example:
```text
A2:C6
C2:B2
```

Finally, start the server or add it to system autorun (e.g., via systemd).
