#pragma once
#include <Arduino.h>

// ─────────────────────────────────────────────────────────
//  OTA Manager  — ElegantOTA over Wi-Fi
//
//  Flow:
//    1. begin(ssid, pass) connects to saved Wi-Fi network
//    2. If connection fails, opens an Access Point named
//       "DelicaDisplay" so you can still reach the UI
//    3. Visit http://delica-display.local (or AP IP) in a
//       browser, upload new .bin firmware → board reboots
//
//  Hostname: delica-display
//  OTA path: http://delica-display.local/update
// ─────────────────────────────────────────────────────────

#define OTA_HOSTNAME  "delica-display"
#define OTA_AP_SSID   "DelicaDisplay"
#define OTA_AP_PASS   "delica1234"

class OtaManager {
public:
    // Call once after config is loaded.
    // ssid/pass from saved config; may be empty strings.
    void begin(const char* ssid, const char* pass);

    // Call every loop() iteration to handle OTA requests.
    void handle();

    // True if connected to a Wi-Fi network (vs AP mode).
    bool isConnected() const { return _connected; }

    // Returns current IP as string for display on settings screen.
    String ipAddress() const;

private:
    bool _connected = false;
    bool _apMode    = false;
};
