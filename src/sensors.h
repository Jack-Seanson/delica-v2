#pragma once
#include <Arduino.h>

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
    void update();

    // Returns latest readings (call update() first)
    const SensorData& getData() const { return _data; }

    // Convenience conversions
    static float toC(float fahrenheit) { return (fahrenheit - 32.0f) * 5.0f / 9.0f; }
    static float toF(float celsius)    { return celsius * 9.0f / 5.0f + 32.0f; }

private:
    SensorData _data;
    unsigned long _lastUpdate = 0;
};
