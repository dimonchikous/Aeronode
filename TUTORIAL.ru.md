# Инструкция

## Оффлайн
### Сборка устройства
Для начала нужно купить все необходимые компоненты:
* ESP32-WROOM-32 devkit v1 (Микроконтроллер)
* BME280 (Измеряет температуру, влажность, давление)
* SCD-40 (Измеритель CO2)
* Oled-экран ssd1306 128x64p (Белого цвета) 

<span style="color:red">**ОБРАТИТЕ ВНИМАНИЕ! ВСЕ КОМПОНЕНТЫ ДОЛЖНЫ БЫТЬ НА I2C ШИНЕ!**</span>

При покупке обратите внимание на то, как выглядит bme280, потому что из-за сходства с bmp280 часто происходит путаница!
<img src="img/BMPvsBME.jpeg" alt="bme and bmp" width="50%" />

Дальше вы должны осуществить подключение всех модулей в одно устройство
![Схема подключения на рисунке](img/connectionScheme.png)

1. 3.3v(esp32) -> VIN(bme280) -> VDD(SCD-40, может называться VCC) -> VCC(ssd1306)
2. GND(esp32) -> GND(bme280) -> GND(SCD-40) -> GND(ssd1306)
3. D21(esp32) -> SDA(bme280) -> SDA(SCD-40) -> SDA(ssd1306)
4. D22(esp32) -> SCL(bme280) -> SCL(SCD-40) -> SCL(ssd1306)

### Прошивка
#### Platformio метод
1. Скопируйте репозиторий 
```bash
git clone https://github.com/dimonchikous/Aeronode.git
```
2. Перейдите в каталог Aeronode/esp32/
3. (Необязательно) Отконфигурируйте файлы lib/AeronodeConfig/AeronodeConfig.h lib/AeroDisplay/AeroDisplay.h
4. Соедините esp32 с компьютером по USB
5. Скомпилируйте и отправьте код на esp32
```bash
pio run -t upload
```
#### esptool метод
1. Установите Python
2. Установите esptool
```bash
pip install esptool
```
3. Подключите esp32 к компьютеру. Проверьте порты

Просмотр портов на Linux: 
```bash
ls /dev/ttyUSB*
```
Если ничего не нашлось
```bash
ls /dev/ttyACM*
```
После этого вы увидите порт, занимаемый esp32. Скорее всего это /dev/ttyUSB0

Просмотр портов на Windows:
* Откройте Диспетчер задач
* Загляните в раздел "Порты"(COM или LPT).
Обычно это COM3, COM4

4. Скачайте файл "Aeronode.bin" из Releases
5. Прошейте устройство
```bash
esptool --chip esp32 --port <ВашПорт> --baud 921600 write_flash 0x0 Aeronode.bin
```
(Замените <ВашПорт> на ваш порт)

#### Adafruit WebSerial метод
1. Подключите esp32 к компьютеру. Проверьте, чтобы никто не использовал порт esp32
2. Из Releases скачайте файл Aeronode.bin
3. Откройте [Adafruit WebSerial ESPTool](https://adafruit.github.io/Adafruit_WebSerial_ESPTool/) 
4. Нажмите connect и в появившемся окне выберите вашу плату(например USB Serial)
5. В появившемся выше списке нажимаем на кнопку "Choose a file" и выбираем Aeronode.bin
    * Убедитесь, что offset выставлен на 0x0
6. Нажмите "Program" и дождитесь полной загрузки, после этого нажмите reset на esp32 или переподключите его

#### Дополнительно
* Если вам не помогли способы выше, то напишите об этом в issues или попробуйте другие способы.
