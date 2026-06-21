/*
 * Delica L300 Temperature Display v2
 * Waveshare ESP32-S3-Touch-LCD-2.1
 *
 * Key differences from v1:
 *  - ESP32_Display_Panel Board class replaces manual DisplayManager
 *  - LVGL runs in its own FreeRTOS task (no lv_timer_handler in loop)
 *  - Touch long-press via LVGL indev (no INT pin polling needed)
 *  - Backlight via board->getBacklight()->setBrightness()
 *  - DS18B20 on GPIO4, DHT22 on GPIO44 (clear of I2C pins)
 */

#include <Arduino.h>
#include <esp_display_panel.hpp>
#include <lvgl.h>
#include "lvgl_v8_port.h"
#include "config.h"
#include "sensors.h"
#include "ota.h"
#include "ui_main.h"
#include "ui_settings.h"
#include "ble_weather.h"

using namespace esp_panel::drivers;
using namespace esp_panel::board;

// ─────────────────────────────────────────────────────────
//  Global singletons
// ─────────────────────────────────────────────────────────
static Board              *board = nullptr;
static SensorManager       sensors;
static OtaManager          ota;
static UiMain              uiMain;
static UiSettings          uiSettings;
static AppConfig           cfg;
static BleWeatherReceiver  bleWeather;

enum Screen { SCREEN_MAIN, SCREEN_SETTINGS };
static Screen activeScreen = SCREEN_MAIN;

// ─────────────────────────────────────────────────────────
//  Backlight helper
// ─────────────────────────────────────────────────────────
static void setBrightness(uint8_t percent) {
    if (board && board->getBacklight()) {
        board->getBacklight()->setBrightness(percent);
    }
}

// ─────────────────────────────────────────────────────────
//  Navigation helpers
// ─────────────────────────────────────────────────────────
static void openSettings() {
    activeScreen = SCREEN_SETTINGS;
    lvgl_port_lock(-1);
    lv_scr_load_anim(uiSettings.screen(), LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
    lvgl_port_unlock();
    Serial.println("[App] → Settings screen");
}

static void closeSettings(const AppConfig& updatedCfg) {
    bool changed = (memcmp(&cfg, &updatedCfg, sizeof(AppConfig)) != 0);
    cfg = updatedCfg;

    if (changed) {
        ConfigManager::save(cfg);
        setBrightness(cfg.brightness);
        Serial.println("[App] Config saved");
        ota.begin(cfg.wifiSSID, cfg.wifiPass);
        lvgl_port_lock(-1);
        uiSettings.setIpAddress(ota.ipAddress());
        lvgl_port_unlock();
    }

    activeScreen = SCREEN_MAIN;
    lvgl_port_lock(-1);
    lv_scr_load_anim(uiMain.screen(), LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
    lvgl_port_unlock();
    Serial.println("[App] → Main screen");
}

// ─────────────────────────────────────────────────────────
//  Arduino setup()
// ─────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.println("\n========================================");
    Serial.println("  Delica L300 Temperature Display v2.0");
    Serial.println("========================================");

    // 1. Load persisted settings
    ConfigManager::load(cfg);

    // 2. Init board (LCD + touch + backlight)
    Serial.println("[Board] Initializing...");
    board = new Board();
    board->init();

    auto lcd = board->getLCD();
    auto lcd_bus = lcd->getBus();
    if (lcd_bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
        lcd->configFrameBufferNumber(2);
        static_cast<BusRGB *>(lcd_bus)->configRGB_BounceBufferSize(lcd->getFrameWidth() * 10);
    }

    assert(board->begin());
    setBrightness(cfg.brightness);
    Serial.println("[Board] Ready");

    // 3. Init LVGL + touch
    Serial.println("[LVGL] Initializing...");
    lvgl_port_init(board->getLCD(), board->getTouch());
    Serial.println("[LVGL] Ready");

    // 4. Build both screens (pre-built, not yet visible)
    lvgl_port_lock(-1);
    uiMain.build(openSettings);
    uiSettings.build(cfg, closeSettings);

    // Show main screen
    lv_scr_load(uiMain.screen());
    lvgl_port_unlock();

    // 5. Init sensors in background task (keeps sensor blocking off main/LVGL core)
    sensors.begin();
    sensors.startTask();

    // 6. Connect to Wi-Fi / start OTA
    ota.begin(cfg.wifiSSID, cfg.wifiPass);
    lvgl_port_lock(-1);
    uiSettings.setIpAddress(ota.ipAddress());
    lvgl_port_unlock();

    // 7. BLE weather receiver (phone → outside temp)
    bleWeather.begin(&sensors);

    // 8. Headlight auto-dim (V2 optional)
#if HEADLIGHT_PIN >= 0
    pinMode(HEADLIGHT_PIN, INPUT);
    Serial.printf("[App] Headlight auto-dim on GPIO %d\n", HEADLIGHT_PIN);
#endif

    Serial.println("[App] Setup complete. Running...");
}

// ─────────────────────────────────────────────────────────
//  Long-press via touch driver polling (v2: CST816S via board)
// ─────────────────────────────────────────────────────────
static void handleTouch() {
    static bool     pressing       = false;
    static uint32_t pressStart     = 0;
    static uint32_t lastTransition = 0;

    if (!board || !board->getTouch()) return;

    // Read touch points: readRawData then getPoints
    auto *tp = board->getTouch();
    tp->readRawData(1, 0, 0);
    esp_panel::drivers::TouchPoint pts[1];
    int npts = tp->getPoints(pts, 1);
    bool touched = (npts > 0);

    if (touched && !pressing) {
        pressing   = true;
        pressStart = millis();
        Serial.println("[Touch] Press start");
    } else if (!touched && pressing) {
        pressing = false;
        Serial.println("[Touch] Released");
    } else if (touched && pressing) {
        uint32_t now = millis();
        if ((now - pressStart) >= LONGPRESS_MS && (now - lastTransition) >= 3000) {
            pressing       = false;
            lastTransition = now;
            Serial.println("[Touch] Long-press FIRE");
            if (activeScreen == SCREEN_MAIN) openSettings();
            else {
                AppConfig tmp = cfg;
                closeSettings(tmp);
            }
        }
    }
}

// ─────────────────────────────────────────────────────────
//  Arduino loop()
// ─────────────────────────────────────────────────────────
void loop() {
    // LVGL runs in its own FreeRTOS task — no lv_timer_handler needed here

    // Touch long-press detection
    handleTouch();

    // OTA handler
    ota.handle();

    // BLE weather — poll phone for outside temp every 15 s
    bleWeather.tick();

    // Push fresh data to main screen (only on change)
    // sensors.update() runs in background task on core 0
    if (activeScreen == SCREEN_MAIN) {
        static float lastExtF   = -999.0f;
        static float lastIntF   = -999.0f;
        static bool  lastExtOk  = true;
        static bool  lastIntOk  = true;

        SensorData d = sensors.getData();
        bool changed = (d.externalTempF  != lastExtF)  ||
                       (d.internalTempF  != lastIntF)  ||
                       (d.externalValid  != lastExtOk) ||
                       (d.internalValid  != lastIntOk);
        if (changed) {
            lastExtF  = d.externalTempF;
            lastIntF  = d.internalTempF;
            lastExtOk = d.externalValid;
            lastIntOk = d.internalValid;
            lvgl_port_lock(-1);
            uiMain.update(d, cfg);
            lvgl_port_unlock();
        }
    }

    // Headlight auto-dim
#if HEADLIGHT_PIN >= 0
    {
        static bool lastHeadlight = false;
        bool headlights = digitalRead(HEADLIGHT_PIN) == HIGH;
        if (headlights != lastHeadlight) {
            lastHeadlight = headlights;
            if (headlights) {
                setBrightness(NIGHT_BRIGHTNESS);
                Serial.println("[App] Headlights ON → dim");
            } else {
                setBrightness(cfg.brightness);
                Serial.println("[App] Headlights OFF → restore");
            }
        }
    }
#endif

    delay(10);
}
