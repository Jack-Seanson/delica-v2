# Delica L300 Temperature Display v2

An ESP32-S3 temperature display built for a **Mitsubishi Delica L300** (and any 12V vehicle). Displays inside and outside temperatures with dynamic weather-scene backgrounds on a **Waveshare 2.1" round touch LCD**.

![Scene backgrounds change with temperature](https://img.shields.io/badge/display-480×480%20round%20touch%20LCD-blue) ![Platform](https://img.shields.io/badge/platform-ESP32--S3-green) ![Framework](https://img.shields.io/badge/framework-Arduino%20%2B%20LVGL%20v8-orange)

---

## Features

- 🌡️ **Outside temp** (top) — via BLE from the companion Android app (real-time weather from GPS location) **or** DS18B20 waterproof probe via 1-Wire
- 🌡️ **Inside temp** (bottom) — DHT22 cabin sensor (temp + humidity)
- 📱 **BLE weather receiver** — connects to the [Delica Weather Android app](#companion-android-app) every 15 seconds to get the real outside air temperature via Open-Meteo (no API key required)
- 🎨 **Dynamic backgrounds** change based on outside temperature:
  - ❄️ Frozen (< 32°F)
  - 🌧️ Cold (< 50°F)
  - ☁️ Cloudy (< 65°F)
  - ☀️ Sunny (< 85°F)
  - 🔥 Hot (≥ 85°F)
- ⚙️ **Settings screen** (long-press): °F/°C toggle, brightness, Wi-Fi selection
- 📡 **OTA firmware updates** over Wi-Fi via ArduinoOTA — no USB cable needed once deployed
- 🔆 **Headlight auto-dim** support (optional GPIO input)

---

## Hardware

| Component | Part |
|-----------|------|
| Microcontroller | [Waveshare ESP32-S3-Touch-LCD-2.1](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-2.1) |
| Display | 480×480 round RGB LCD (built-in) |
| Touch | CST816S capacitive touch (built-in) |
| External temp | DS18B20 waterproof 1-Wire probe → **GPIO 4** |
| Internal temp/humidity | DHT22 → **GPIO 44** |
| Power | 12V → 5V buck converter (USB-C to board) |

---

## Wiring

```
ESP32-S3 Board
├── GPIO 4   → DS18B20 data (+ 4.7kΩ pull-up to 3.3V)
├── GPIO 44  → DHT22 data   (+ 10kΩ pull-up to 3.3V)
├── 3.3V     → DS18B20 VCC, DHT22 VCC
└── GND      → DS18B20 GND, DHT22 GND
```

---

## Software

- **Framework**: Arduino (ESP-IDF via PlatformIO)
- **Display library**: [ESP32_Display_Panel](https://github.com/esp-arduino-libs/ESP32_Display_Panel)
- **UI library**: [LVGL v8.4](https://github.com/lvgl/lvgl)
- **Sensors**: DallasTemperature (DS18B20), DHT (DHT22)
- **OTA**: ArduinoOTA (built-in Arduino ESP32)

### Key technical fixes

- **No display tearing**: `LVGL_PORT_AVOID_TEARING_MODE=3` (double framebuffer + direct mode for RGB LCD)
- **No display shudder**: Sensors run in a FreeRTOS task pinned to **core 0** (`xTaskCreatePinnedToCore`), completely isolated from LVGL on core 1. DS18B20 uses async non-blocking conversion (`setWaitForConversion(false)`)
- **Thread safety**: `SensorData` reads protected with a FreeRTOS mutex

---

## Building & Flashing

### Requirements
- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)
- USB-C cable (first flash only)

### First flash (USB)
```bash
cd delica-v2
pio run --target upload
```

### Wi-Fi OTA (subsequent updates)
Once the device is connected to Wi-Fi, the IP address appears in Settings. Then:
```bash
pio run --target upload --upload-port <ip-address>
```
Or navigate to `http://<ip-address>` in your browser to upload a `.bin` manually.

---

## Configuration

Edit `platformio.ini` to change Wi-Fi credentials or GPIO pins:

```ini
build_flags =
    -D WIFI_SSID1=\"YourHomeNetwork\"
    -D WIFI_PASS1=\"YourPassword\"
    -D WIFI_SSID2=\"YourMobileHotspot\"
    -D WIFI_PASS2=\"HotspotPassword\"
    -D WIFI_NAME1=\"Home\"
    -D WIFI_NAME2=\"Hotspot\"
    -D PIN_DS18B20=4       ; external temp sensor GPIO
    -D PIN_DHT22=44        ; internal temp/humidity GPIO
    -D HEADLIGHT_PIN=-1    ; set to GPIO for auto-dim (-1 = disabled)
```

The device automatically tries both networks on boot and connects to whichever is in range.

---

## Settings Screen

Access by **long-pressing** (1.5s) the main display.

| Control | Function |
|---------|----------|
| °F / °C switch | Toggle temperature units |
| Brightness slider | Adjust display brightness (10–100%) |
| Wi-Fi buttons | Select which network to connect (saved to NVS flash) |
| IP address | Shows current OTA update URL |
| Save / Back | Save changes and return to main display |

---

## Project Structure

```
src/
├── main.cpp          — Setup, loop, touch handling, screen transitions
├── config.h/cpp      — AppConfig struct, NVS persistence (Preferences)
├── sensors.h/cpp     — DS18B20 + DHT22, FreeRTOS task on core 0
├── ui_main.h/cpp     — Main temperature display screen
├── ui_settings.h/cpp — Settings screen
├── ota.h/cpp         — ArduinoOTA Wi-Fi manager
├── lvgl_v8_port.*    — LVGL ↔ ESP32_Display_Panel bridge
├── img_bg_*.h        — Background scene images (C array, LVGL format)
└── img_van.h         — Delica van pixel art
```

---

## Companion Android App

The **Delica Weather** Android app ([`DelicaWeather/`](https://github.com/Jack-Seanson/DelicaWeather)) runs as a background service on your phone and broadcasts the real outside air temperature to the display via BLE.

### How it works
1. App fetches current temperature from [Open-Meteo](https://open-meteo.com/) using your phone's GPS location (no API key required)
2. App advertises itself as a BLE GATT peripheral named **"DelicaWeather"**
3. ESP32 scans for the device every 15 seconds, connects, reads the temperature characteristic, then disconnects
4. Temperature is displayed in the **OUTSIDE** field and the background scene updates automatically

### BLE protocol
| | Value |
|--|--|
| Service UUID | `12345678-1234-1234-1234-123456789abc` |
| Characteristic UUID | `12345678-1234-1234-1234-123456789abd` |
| Value format | UTF-8 string, e.g. `"75.2"` (°F) |
| Device name | `"DelicaWeather"` |

### Requirements
- Android 8.0+ (Oreo)
- Bluetooth + Location permissions
- Phone within ~30 feet of the display

---

## License

MIT
