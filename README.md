# Aeronode - your assistant in monitoring indoor air.
[English](README.md) | [Русский](README.ru.md)

[Instructions](TUTORIAL.md)

## Main idea.
The air in an apartment is rarely monitored; most often, it is checked poorly and based purely on personal feelings. This can frequently harm human health.

This project is designed to solve this problem, specifically to monitor the condition of indoor air. Also, when the device is connected to a server, it will be able to send regular notifications about the weather in the user's city, notify about air quality, and act as an improvised alarm in case of critical readings.

---

## How it works.

### Offline
In **offline** mode, the device independently displays information using an OLED screen.

The device itself is based on the ESP32-WROOM32 devkit v1.

In addition to this, the following components are used:
* SCD-40 for measuring CO2 levels.
* BME280 for measuring temperature, pressure, and air humidity.
* SSD1306 128x64 OLED screen - for displaying information.

### Online
<img src="img/EN/workinprogress.png" alt="Work in progress" width="35%" />

![Online workflow diagram](img/EN/roadmap.png)
*(Since internet support is still in development, the diagram is not 100% accurate)*

Once a minute, the device sends a JSON packet to a server with a Telegram bot. The bot tracks these packets and, upon the user's request, sends the data, allowing you to view the readings while away from home. There is also the ability to set a schedule for receiving readings, view the weather, and enable an improvised alarm that triggers at high temperatures and/or high CO2 levels. The main requirement is to have a server or a spare, unused computer lying around.

Telegram is used as the messenger for the bot.

## Other information
[github](https://github.com/dimonchikous/Aeronode) | [codeberg](https://codeberg.org/dimonchikous/Aeronode)
