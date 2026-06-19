#include "config.h"

Preferences ConfigManager::_prefs;

void ConfigManager::load(AppConfig &cfg) {
    _prefs.begin(PREF_NAMESPACE, true); // read-only

    cfg.useCelsius = _prefs.getBool(PREF_USE_CELSIUS, DEFAULT_CELSIUS);
    cfg.brightness = _prefs.getUChar(PREF_BRIGHTNESS, DEFAULT_BRIGHTNESS);

    String ssid = _prefs.getString(PREF_WIFI_SSID, DEFAULT_WIFI_SSID);
    String pass = _prefs.getString(PREF_WIFI_PASS, DEFAULT_WIFI_PASS);
    strncpy(cfg.wifiSSID, ssid.c_str(), sizeof(cfg.wifiSSID) - 1);
    strncpy(cfg.wifiPass, pass.c_str(), sizeof(cfg.wifiPass) - 1);

    _prefs.end();

    Serial.printf("[Config] Loaded: celsius=%d brightness=%d ssid=%s\n",
                  cfg.useCelsius, cfg.brightness, cfg.wifiSSID);
}

void ConfigManager::save(const AppConfig &cfg) {
    _prefs.begin(PREF_NAMESPACE, false); // read-write

    _prefs.putBool(PREF_USE_CELSIUS, cfg.useCelsius);
    _prefs.putUChar(PREF_BRIGHTNESS,  cfg.brightness);
    _prefs.putString(PREF_WIFI_SSID,  cfg.wifiSSID);
    _prefs.putString(PREF_WIFI_PASS,  cfg.wifiPass);

    _prefs.end();

    Serial.printf("[Config] Saved: celsius=%d brightness=%d ssid=%s\n",
                  cfg.useCelsius, cfg.brightness, cfg.wifiSSID);
}
