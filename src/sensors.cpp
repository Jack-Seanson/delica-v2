#include "sensors.h"
#include "config.h"
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ─── DHT22 (internal) ───────────────────────────────────
#if PIN_DHT22 >= 0
static DHT dht(PIN_DHT22, DHT22);
#endif

// ─── DS18B20 (external, waterproof) ─────────────────────
#if PIN_DS18B20 >= 0
static OneWire oneWire(PIN_DS18B20);
static DallasTemperature ds18b20(&oneWire);
#endif

void SensorManager::startTask() {
    _mutex = xSemaphoreCreateMutex();
    // Pin sensor task to core 0; LVGL task runs on core 1 (Arduino default)
    xTaskCreatePinnedToCore(sensorTask, "sensors", 4096, this, 1, nullptr, 0);
    Serial.println("[Sensors] Background task started on core 0");
}

void SensorManager::sensorTask(void *arg) {
    SensorManager *self = static_cast<SensorManager*>(arg);
    for (;;) {
        self->update();
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void SensorManager::begin() {
    Serial.println("[Sensors] Initializing...");

#if PIN_DHT22 >= 0
    dht.begin();
    Serial.printf("[Sensors] DHT22 enabled on pin %d\n", PIN_DHT22);
#else
    Serial.println("[Sensors] DHT22 disabled (PIN_DHT22=-1)");
#endif

#if PIN_DS18B20 >= 0
    ds18b20.begin();
    // Non-blocking mode: requestTemperatures() returns immediately,
    // we read the result 800ms later on the next update cycle.
    // This prevents the 750ms blocking stall that garbles the RGB LCD.
    ds18b20.setWaitForConversion(false);
    Serial.printf("[Sensors] DS18B20 enabled on pin %d (async mode)\n", PIN_DS18B20);
#else
    Serial.println("[Sensors] DS18B20 disabled (PIN_DS18B20=-1)");
#endif

    _data = {};
}

void SensorManager::update() {
    unsigned long now = millis();

#if PIN_DS18B20 >= 0
    // ── DS18B20 async state machine ───────────────────────
    // Phase 1: kick off conversion (non-blocking, returns immediately)
    // Phase 2: 800ms later, read result then kick next conversion
    if (!_ds18b20Started) {
        // First call: start a conversion
        if (now - _lastUpdate >= SENSOR_UPDATE_MS) {
            ds18b20.requestTemperatures();
            _ds18b20Started    = true;
            _ds18b20RequestAt  = now;
        }
    } else {
        // Conversion takes ~750ms; wait 800ms before reading
        if (now - _ds18b20RequestAt >= 800) {
            float ext = ds18b20.getTempFByIndex(0);
            if (_mutex) xSemaphoreTake(_mutex, portMAX_DELAY);
            if (ext > -100.0f) {
                _data.externalTempF = ext;
                _data.externalValid = true;
            } else {
                _data.externalValid = false;
            }
            if (_mutex) xSemaphoreGive(_mutex);
            if (ext > -100.0f) {
                Serial.printf("[Sensors] External: %.1f°F\n", ext);
            } else {
                Serial.printf("[Sensors] External FAIL (raw=%.1f) — check DS18B20 wiring on GPIO%d\n", ext, PIN_DS18B20);
            }
            // Kick next conversion
            ds18b20.requestTemperatures();
            _ds18b20RequestAt = now;
            _lastUpdate       = now;
        }
    }
#else
    if (_mutex) xSemaphoreTake(_mutex, portMAX_DELAY);
    _data.externalValid = false;
    if (_mutex) xSemaphoreGive(_mutex);
#endif

#if PIN_DHT22 >= 0
    // ── DHT22: internal temp + humidity ──────────────────
    if (now - _lastDHTUpdate >= SENSOR_UPDATE_MS) {
        _lastDHTUpdate = now;
        float h = dht.readHumidity();
        float t = dht.readTemperature(true);
        if (_mutex) xSemaphoreTake(_mutex, portMAX_DELAY);
        if (!isnan(h) && !isnan(t)) {
            _data.internalTempF    = t;
            _data.internalHumidity = h;
            _data.internalValid    = true;
        } else {
            _data.internalValid = false;
        }
        if (_mutex) xSemaphoreGive(_mutex);
        if (!isnan(t)) {
            Serial.printf("[Sensors] Internal: %.1f°F  %.1f%%RH\n", t, h);
        } else {
            Serial.printf("[Sensors] Internal FAIL (t=%.1f h=%.1f) — check DHT22 wiring on GPIO%d\n", t, h, PIN_DHT22);
        }
    }
#else
    if (_mutex) xSemaphoreTake(_mutex, portMAX_DELAY);
    _data.internalValid = false;
    if (_mutex) xSemaphoreGive(_mutex);
#endif
}
