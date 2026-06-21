#include "ble_weather.h"
#include <NimBLEDevice.h>

void BleWeatherReceiver::begin(SensorManager* sensors) {
    _sensors = sensors;
    NimBLEDevice::init("");
    Serial.println("[BLE] Initialized — scanning for DelicaWeather");
}

void BleWeatherReceiver::tick() {
    if (!_sensors) return;
    _sensors->tickBleExpiry();

    unsigned long now = millis();
    if (now - _lastPoll < BLE_POLL_MS) return;
    _lastPoll = now;

    _connected = scan_and_read();
    if (!_connected) {
        Serial.println("[BLE] No DelicaWeather device found");
    }
}

bool BleWeatherReceiver::scan_and_read() {
    NimBLEScan* scan = NimBLEDevice::getScan();
    scan->setActiveScan(false);
    scan->setInterval(80);
    scan->setWindow(40);

    NimBLEScanResults results = scan->start(3, false);  // 3s scan

    NimBLEAddress targetAddr;
    bool found = false;
    for (int i = 0; i < results.getCount(); i++) {
        NimBLEAdvertisedDevice dev = results.getDevice(i);
        if (dev.getName() == BLE_DEVICE_NAME ||
            dev.isAdvertisingService(NimBLEUUID(BLE_SERVICE_UUID))) {
            targetAddr = dev.getAddress();
            found = true;
            break;
        }
    }
    scan->clearResults();

    if (!found) return false;

    NimBLEClient* client = NimBLEDevice::createClient();
    client->setConnectionParams(12, 12, 0, 51);

    if (!client->connect(targetAddr)) {
        NimBLEDevice::deleteClient(client);
        Serial.println("[BLE] Connect failed");
        return false;
    }

    NimBLERemoteService* svc = client->getService(BLE_SERVICE_UUID);
    if (!svc) {
        client->disconnect();
        NimBLEDevice::deleteClient(client);
        Serial.println("[BLE] Service not found");
        return false;
    }

    NimBLERemoteCharacteristic* ch = svc->getCharacteristic(BLE_CHAR_UUID);
    if (!ch || !ch->canRead()) {
        client->disconnect();
        NimBLEDevice::deleteClient(client);
        Serial.println("[BLE] Characteristic not found / not readable");
        return false;
    }

    std::string val = ch->readValue();
    client->disconnect();
    NimBLEDevice::deleteClient(client);

    if (val.empty()) return false;

    float tempF = atof(val.c_str());
    if (tempF < -60.0f || tempF > 160.0f) {
        Serial.printf("[BLE] Temp out of range: %.1f°F — ignored\n", tempF);
        return false;
    }

    _sensors->setExternalTemp(tempF);
    Serial.printf("[BLE] Got temp: %.1f°F\n", tempF);
    return true;
}
