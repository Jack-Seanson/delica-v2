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
    Serial.printf("[Sensors] DS18B20 enabled on pin %d\n", PIN_DS18B20);
#else
    Serial.println("[Sensors] DS18B20 disabled (PIN_DS18B20=-1)");
#endif

    _data = {};
}

void SensorManager::update() {
    unsigned long now = millis();
    if (now - _lastUpdate < SENSOR_UPDATE_MS) return;
    _lastUpdate = now;

#if PIN_DHT22 >= 0
    // ── DHT22: internal temp + humidity ──────────────────
    float h = dht.readHumidity();
    float t = dht.readTemperature(true); // true = Fahrenheit

    if (!isnan(h) && !isnan(t)) {
        _data.internalTempF    = t;
        _data.internalHumidity = h;
        _data.internalValid    = true;
        Serial.printf("[Sensors] Internal: %.1f°F  %.1f%%RH\n", t, h);
    } else {
        _data.internalValid = false;
    }
#else
    _data.internalValid = false;
#endif

#if PIN_DS18B20 >= 0
    // ── DS18B20: external temp ────────────────────────────
    ds18b20.requestTemperatures();
    float ext = ds18b20.getTempFByIndex(0);

    if (ext > -100.0f) {   // DEVICE_DISCONNECTED_F = -196.6°F; anything below -100°F is invalid
        _data.externalTempF  = ext;
        _data.externalValid  = true;
        Serial.printf("[Sensors] External: %.1f°F\n", ext);
    } else {
        _data.externalValid = false;
    }
#else
    _data.externalValid = false;
#endif
}
