#pragma once
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// ─────────────────────────────────────────────
//  Sensor Manager
//  - DHT22 on PIN_DHT22  → internal temp + humidity
//  - DS18B20 on PIN_DS18B20 → external (waterproof) temp
// ─────────────────────────────────────────────

struct SensorData {
    float internalTempF;   // °F
    float internalHumidity; // %RH
    float externalTempF;   // °F
    bool internalValid;
    bool externalValid;
};

class SensorManager {
public:
    void begin();
    // Call update() from loop() if NOT using startTask()
    void update();
    // Start a background FreeRTOS task — sensors run on core 0,
    // completely isolated from the LVGL/display core.
    void startTask();

    // Thread-safe: inject external temperature from BLE weather source
    void setExternalTemp(float tempF) {
        if (_mutex) xSemaphoreTake(_mutex, portMAX_DELAY);
        _data.externalTempF = tempF;
        _data.externalValid = true;
        _bleLastUpdate = millis();
        if (_mutex) xSemaphoreGive(_mutex);
    }

    // Call periodically to expire BLE data if phone goes out of range (60s timeout)
    void tickBleExpiry() {
        if (_data.externalValid && _bleLastUpdate > 0) {
            if ((millis() - _bleLastUpdate) > 60000UL) {
                if (_mutex) xSemaphoreTake(_mutex, portMAX_DELAY);
                _data.externalValid = false;
                if (_mutex) xSemaphoreGive(_mutex);
            }
        }
    }

    // Thread-safe: returns a snapshot of the latest readings
    SensorData getData() const {
        SensorData copy;
        if (_mutex) xSemaphoreTake(_mutex, portMAX_DELAY);
        copy = _data;
        if (_mutex) xSemaphoreGive(_mutex);
        return copy;
    }

    // Convenience conversions
    static float toC(float fahrenheit) { return (fahrenheit - 32.0f) * 5.0f / 9.0f; }
    static float toF(float celsius)    { return celsius * 9.0f / 5.0f + 32.0f; }

private:
    SensorData      _data;
    SemaphoreHandle_t _mutex           = nullptr;
    unsigned long   _lastUpdate        = 0;
    unsigned long   _lastDHTUpdate     = 0;
    unsigned long   _ds18b20RequestAt  = 0;
    unsigned long   _bleLastUpdate     = 0;   // millis() of last BLE injection
    bool            _ds18b20Started    = false;

    static void sensorTask(void *arg);
};
