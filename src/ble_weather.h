#pragma once
#include <Arduino.h>
#include "sensors.h"

// ─────────────────────────────────────────────────────────
//  BLE Weather Receiver
//
//  Listens for a BLE GATT characteristic from the Delica
//  Weather Android app. The phone acts as a BLE peripheral
//  (server) and advertises:
//
//    Service UUID : 12345678-1234-1234-1234-123456789abc
//    Char UUID    : 12345678-1234-1234-1234-123456789abd
//
//  The characteristic value is a UTF-8 string: "72.4"  (°F)
//  The ESP32 connects, reads the value, then disconnects.
//  It reconnects every BLE_POLL_MS milliseconds.
//
//  All temp → SensorData injection goes through
//  SensorManager::setExternalTemp() (thread-safe).
// ─────────────────────────────────────────────────────────

#define BLE_SERVICE_UUID  "12345678-1234-1234-1234-123456789abc"
#define BLE_CHAR_UUID     "12345678-1234-1234-1234-123456789abd"
#define BLE_DEVICE_NAME   "DelicaWeather"
#define BLE_POLL_MS       15000UL   // re-scan every 15 s

class BleWeatherReceiver {
public:
    void begin(SensorManager* sensors);
    void tick();   // call from loop()

    bool connected() const { return _connected; }

private:
    SensorManager* _sensors     = nullptr;
    bool           _connected   = false;
    unsigned long  _lastPoll    = 0;

    bool scan_and_read();
};
