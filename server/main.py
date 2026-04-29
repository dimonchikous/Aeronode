import asyncio;
import os;
from contextlib import asynccontextmanager;

from fastapi import FastAPI;
from pydantic import BaseModel;
import uvicorn;

from aiogram import Bot, Dispatcher, types, F;
from aiogram.types import ReplyKeyboardMarkup, KeyboardButton;
from aiogram.filters import Command;
from dotenv import load_dotenv;
from datetime import datetime;

import aiohttp;

#env config
load_dotenv();

BOT_TOKEN = os.getenv("BOT_TOKEN");

USE_WHITELIST = os.getenv("USE_WHITELIST", "False").lower() == "true";
USE_WEATHER = os.getenv("USE_WEATHER", "False").lower() == "true";
USE_CRITICAL_CO2_DETECTOR = os.getenv("USE_CRITICAL_CO2_DETECTOR", "False").lower() == "true";
USE_TIME_NOTIFICATION = os.getenv("USE_TIME_NOTIFICATION", "False").lower() == "true";
USE_SLEEPTIME = os.getenv("USE_SLEEPTIME", "False").lower() == "true";

WEATHER_CITY=os.getenv("WEATHER_CITY");

MAX_CRITICAL_VALUE=int(os.getenv("MAX_CRITICAL_VALUE", "1100"));
NORMAL_CRITICAL_VALUE=int(os.getenv("NORMAL_CRITICAL_VALUE", "800"));

NOTIFICATION_TIME=os.getenv("NOTIFICATION_TIME", "06:00");
NOTIFICATION_MSG_START= os.getenv("NOTIFICATION_MSG_START", "Hello!");

SLEEPTIME = os.getenv("SLEEPTIME", "00:00");

warning_states = {};

async def getWeather(city: str = WEATHER_CITY):
        wUrl = f"https://wttr.in/{city}?format=j1"; #weather url

        try:
            async with aiohttp.ClientSession() as session:
                async with session.get(wUrl, timeout=5) as response:
                    if response.status == 200:
                        wData = await response.json(content_type=None);
                        wCurrent = wData["current_condition"][0];

                        return{
                                "wTemp": wCurrent["temp_C"],
                                "wHum": wCurrent["humidity"],
                                "wDesc": wCurrent["weatherDesc"][0]["value"]
                        }
                    else:
                        print(f"API ERROR: {response.status}")
                        return None;
        except Exception as e:
            print(f"Error while connecting with weather API: {e}");
            return None;

#file reading
def loadListFromFile(filename:str)->set:
    if not os.path.exists(filename):
        return set();
    with open(filename, "r", encoding="utf-8") as f:
        return {line.strip() for line in f.readlines() if line.strip()};

#database
nodesData={};

class AeronodeData(BaseModel):
    name: str;
    temp: int;
    hum: int;
    co2: int;

#bot
bot=Bot(token=BOT_TOKEN);
dp=Dispatcher();

main_kb=ReplyKeyboardMarkup(
        keyboard=[
            [KeyboardButton(text="Display metrics")]
            ],
        resize_keyboard=True
        );

async def sendWarning(message_text: str):
        allowed_users = loadListFromFile("users.txt");
        if not allowed_users:
            return;

        for user_id in allowed_users:
            try:
                await bot.send_message(chat_id=user_id, text=message_text, parse_mode="HTML");
            except Exception as e:
                print(f"ERROR: Cant send alert to {user_id}: {e}");

@dp.message(Command("start"))
async def cmd_startx(message: types.Message):
    allowed_users = loadListFromFile("users.txt");
    if str(message.from_user.id) not in allowed_users:
        await message.answer("ERROR: Not allowed user");
        return;

    await message.answer(
            "Welcome to Aeronode. Press button 'Display metrics' for getting metrics from ESP32.",
            reply_markup=main_kb);

@dp.message(Command("info"))
@dp.message(F.text == "Display metrics")
async def cmd_info(message: types.Message):
    allowed_users = loadListFromFile("users.txt");
    if str(message.from_user.id) not in allowed_users:
        await message.answer("ERROR: Not allowed user");
        return;
    
    weather = None;
    if USE_WEATHER:
        weather = await getWeather(WEATHER_CITY);

    response_text = "Current Aeronode`s data:\n\n";
    if USE_WEATHER and weather:
        response_text += f"Weather: {weather['wTemp']}°C, Humidity: {weather['wHum']}%, {weather['wDesc']}\n";
        response_text += "──────────────\n";

    if not nodesData:
        response_text += "No data from Aeronodes.\n";
    else:
        for node_name, data in nodesData.items():
            isWarningActiveDisplay = False;
            if USE_CRITICAL_CO2_DETECTOR:
                isWarningActiveDisplay = warning_states.get(node_name, False);
            response_text += f"Node name: <code>{node_name}</code>\n";
            response_text += f"Temperature: {data['temp']}°C\n";
            response_text += f"Humidity: {data['hum']}%\n";
            if isWarningActiveDisplay and USE_CRITICAL_CO2_DETECTOR:
                response_text += f"CO2: <b>{data['co2']}ppm</b>\n";
                response_text += f"<i>(High CO2 level! Normal is {NORMAL_CRITICAL_VALUE}ppm)</i>\n";
            else:
                response_text += f"CO2: {data['co2']}ppm\n";
            response_text += "──────────────\n";

    await message.answer(response_text, parse_mode="HTML");

def isSleepTime():
    if not USE_SLEEPTIME and not USE_CRITICAL_CO2_DETECTOR and not USE_TIME_NOTIFICATION:
        print("Missed needed flags");
        return False;

    currentSTime=datetime.now().strftime("%H:%M");

    if SLEEPTIME < NOTIFICATION_TIME:
        return SLEEPTIME <= currentSTime < NOTIFICATION_TIME;
    else:
        return currentSTime >= SLEEPTIME or currentSTime < NOTIFICATION_TIME;

async def alarmClockTask():
    while True:
        if USE_TIME_NOTIFICATION:
            currentTime = datetime.now().strftime("%H:%M");

            if currentTime == NOTIFICATION_TIME:
                alarm_msg = f"{NOTIFICATION_MSG_START} Everyday info:\n\n";
                
                alarmWeather = None;
                if USE_WEATHER:
                    alarmWeather = await getWeather(WEATHER_CITY);
                if USE_WEATHER and alarmWeather:
                    alarm_msg += f"Weather: {alarmWeather['wTemp']}°C, Humidity: {alarmWeather['wHum']}%, {alarmWeather['wDesc']}\n";
                    alarm_msg += "──────────────\n";

                if not nodesData:
                    alarm_msg += "No data from Aeronodes\n";
                else:
                    for node_name, data in nodesData.items():
                        isWarningActive = False;
                        if USE_CRITICAL_CO2_DETECTOR:
                            isWarningActive = warning_states.get(node_name, False);

                        alarm_msg += f"Node name: <code>{node_name}</code>\n";
                        alarm_msg += f"Temperature: {data['temp']}°C\n";
                        alarm_msg += f"Humidity: {data['hum']}%\n";
                        if isWarningActive and USE_CRITICAL_CO2_DETECTOR:
                            alarm_msg += f"CO2: <b>{data['co2']}ppm</b>\n";
                            alarm_msg += f"<i>(High CO2 level! Normal is {NORMAL_CRITICAL_VALUE}ppm)</i>\n";
                        else:
                            alarm_msg += f"CO2: {data['co2']}ppm\n";
                        alarm_msg += "──────────────\n";
                await sendWarning(alarm_msg);
                
                await asyncio.sleep(100);

        await asyncio.sleep(10);

@asynccontextmanager
async def lifespan(app: FastAPI):
    polling_task=asyncio.create_task(dp.start_polling(bot));
    alarm_task=asyncio.create_task(alarmClockTask());
    yield;
    
    print("Poweroff");
    polling_task.cancel();
    alarm_task.cancel();

    try:
        await polling_task;
        await alarm_task;
    except asyncio.CancelledError:
        print("Bot succesfully stopped");

    finally:
        await bot.session.close();
        print("Session closed");

app = FastAPI(lifespan=lifespan)

@app.post("/update")
async def receive_update(data: AeronodeData):
    mac_suffix=data.name.split("-")[-1];

    blacklist=loadListFromFile("blacklist.txt");
    whitelist=loadListFromFile("whitelist.txt");

    if mac_suffix in blacklist:
        print(f"Access denied(Node in blacklist.txt): {data.name}");
        return {"status": "rejected", "reason": "blacklisted"}

    if USE_WHITELIST and mac_suffix not in whitelist:
        print(f"Access denied(Node is not in whitelist.txt): {data.name}");
        return {"status": "rejected", "reason": "not_in_whitelist"}

    nodesData[data.name] = data.model_dump();
    if USE_CRITICAL_CO2_DETECTOR:
        nodeName = data.name;
        currentCO2 = data.co2;
        isWarningActive = warning_states.get(nodeName, False);

        if currentCO2 >= MAX_CRITICAL_VALUE and not isWarningActive:
            warning_states[nodeName]=True;
            
            if not isSleepTime():
                warn_msg = f"High CO2 level!\n\n";
                warn_msg += f"Node name: <code>{nodeName}</code>\n";
                warn_msg += f"CO2 level: <b>{currentCO2}ppm</b>\n";
                warn_msg += f"<i>(Normal CO2 level is {NORMAL_CRITICAL_VALUE}ppm)</i>\n";

                await sendWarning(warn_msg);
            else:
                print(f"DEBUG: High CO2 on {nodeName}, but sleep mode activated. Alert suppressed.");

        elif currentCO2 <= NORMAL_CRITICAL_VALUE and isWarningActive:
            warning_states[nodeName]=False;

            if not isSleepTime():
                ok_msg = f"Air cleared.\n\n";
                ok_msg += f"Node name: <code>{nodeName}</code>\n";
                ok_msg += f"CO2 level: {currentCO2}ppm.\n";

                await sendWarning(ok_msg);

    print(f"Data saved: {data.name}")

    return {"status": "success"}

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=5000);
