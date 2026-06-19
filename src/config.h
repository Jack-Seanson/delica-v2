#pragma once
#include <Arduino.h>
#include <Preferences.h>

// ─────────────────────────────────────────────
//  Persistent configuration (stored in NVS)
// ─────────────────────────────────────────────

#define PREF_NAMESPACE   "delica"
#define PREF_USE_CELSIUS "celsius"
#define PREF_BRIGHTNESS  "brightness"
#define PREF_WIFI_SSID   "wifi_ssid"
#define PREF_WIFI_PASS   "wifi_pass"

// Default values
#define DEFAULT_CELSIUS    false   // show °F by default
#define DEFAULT_BRIGHTNESS 80      // 0–100 percent
#define DEFAULT_WIFI_SSID  ""
#define DEFAULT_WIFI_PASS  ""

// Temperature thresholds (all in °F)
#define TEMP_SNOW_BELOW   32.0f   // ❄️ Snow scene
#define TEMP_RAIN_BELOW   50.0f   // 🌧️ Rain / overcast
#define TEMP_CLOUDY_BELOW 65.0f   // ☁️ Cool / cloudy
#define TEMP_SUNNY_BELOW  85.0f   // ☀️ Sunny / green
// >= 85°F → 🔥 Hot / hazy

// Sensor update interval (ms)
#define SENSOR_UPDATE_MS  5000

// Long-press threshold to open settings (ms)
#define LONGPRESS_MS      1500

// ─────────────────────────────────────────────
//  V2 placeholder: headlight auto-dim
// ─────────────────────────────────────────────
// When HEADLIGHT_PIN >= 0 the firmware reads this GPIO:
//   HIGH → headlights ON  → dim to NIGHT_BRIGHTNESS
//   LOW  → headlights OFF → restore day brightness
// Circuit: 12V headlight wire → 10kΩ/3.9kΩ voltage divider → GPIO
//          OR  PC817 optocoupler for electrical isolation (recommended)
#define NIGHT_BRIGHTNESS  20   // % brightness when headlights detected

struct AppConfig {
    bool useCelsius;
    uint8_t brightness;   // 0–100
    char wifiSSID[64];
    char wifiPass[64];
};

class ConfigManager {
public:
    static void load(AppConfig &cfg);
    static void save(const AppConfig &cfg);

private:
    static Preferences _prefs;
};
