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

import aiohttp;

#env config
load_dotenv();

BOT_TOKEN = os.getenv("BOT_TOKEN");

USE_WHITELIST = os.getenv("USE_WHITELIST", "False").lower() == "true";
USE_WEATHER = os.getenv("USE_WEATHER", "False").lower() == "true";

WEATHER_CITY=os.getenv("WEATHER_CITY");
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
            response_text += f"Node name: <code>{node_name}</code>\n";
            response_text += f"Temperature: {data['temp']}°C\n";
            response_text += f"Humidity: {data['hum']}%\n";
            response_text += f"CO2: {data['co2']}ppm\n";
            response_text += "──────────────\n";

    await message.answer(response_text, parse_mode="HTML");


@asynccontextmanager
async def lifespan(app: FastAPI):
    polling_task=asyncio.create_task(dp.start_polling(bot));
    yield;
    
    print("Poweroff");
    polling_task.cancel();

    try:
        await polling_task;
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
        print(f"Acces denied(Node is not in whitelist.txt): {data.name}");
        return {"status": "rejected", "reason": "not_in_whitelist"}

    nodesData[data.name] = data.model_dump();
    print(f"Data saved: {data.name}")

    return {"status": "succes"}

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=5000);
