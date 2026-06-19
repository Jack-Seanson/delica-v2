#include "ota.h"
#include <WiFi.h>
#include <ArduinoOTA.h>

// ── Hardcoded network list ──────────────────────────────────
// Add more entries as needed. Empty PASS = open network.
struct KnownNetwork {
    const char* ssid;
    const char* pass;
};

#ifndef WIFI_SSID1
#define WIFI_SSID1 ""
#define WIFI_PASS1 ""
#endif
#ifndef WIFI_SSID2
#define WIFI_SSID2 ""
#define WIFI_PASS2 ""
#endif

static const KnownNetwork NETWORKS[] = {
    { WIFI_SSID1, WIFI_PASS1 },
    { WIFI_SSID2, WIFI_PASS2 },
};
static const int NUM_NETWORKS = sizeof(NETWORKS) / sizeof(NETWORKS[0]);

static bool tryConnect(const char* ssid, const char* pass, int timeoutMs = 8000) {
    if (!ssid || ssid[0] == '\0') return false;
    Serial.printf("[OTA] Trying %s ...\n", ssid);
    WiFi.begin(ssid, (pass && pass[0]) ? pass : nullptr);
    unsigned long t = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - t) < (unsigned long)timeoutMs) {
        delay(200);
    }
    return WiFi.status() == WL_CONNECTED;
}

void OtaManager::begin(const char* /*ssid*/, const char* /*pass*/) {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(100);

    bool connected = false;
    for (int i = 0; i < NUM_NETWORKS && !connected; i++) {
        connected = tryConnect(NETWORKS[i].ssid, NETWORKS[i].pass);
    }

    if (!connected) {
        Serial.println("[OTA] No known Wi-Fi found — OTA unavailable");
        _ip = "";
        return;
    }

    _ip = WiFi.localIP().toString();
    Serial.printf("[OTA] Connected! IP: %s\n", _ip.c_str());

    // ArduinoOTA: flash via IDE or PlatformIO over Wi-Fi
    ArduinoOTA.setHostname("delica-display");
    ArduinoOTA.onStart([]() {
        Serial.println("[OTA] Update start");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\n[OTA] Done — rebooting");
    });
    ArduinoOTA.onProgress([](unsigned int p, unsigned int t) {
        Serial.printf("[OTA] %u%%\r", (p * 100) / t);
    });
    ArduinoOTA.onError([](ota_error_t err) {
        Serial.printf("[OTA] Error[%u]\n", err);
    });
    ArduinoOTA.begin();
    Serial.printf("[OTA] ArduinoOTA ready at delica-display.local / %s\n", _ip.c_str());
    _started = true;
}

void OtaManager::handle() {
    if (_started) ArduinoOTA.handle();
}

String OtaManager::ipAddress() const {
    return _ip.length() > 0 ? _ip : "not connected";
}
