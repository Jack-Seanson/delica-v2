#pragma once
#include <lvgl.h>
#include "config.h"

// ─────────────────────────────────────────────────────────
//  Settings Screen (opened by long-press on main screen)
//
//  Controls:
//    🌡️  °F / °C  toggle switch
//    ☀️  Brightness slider (10–100%)
//    📶  Wi-Fi SSID  text field
//    🔑  Wi-Fi Password text field
//    💾  Save button  → saves config + reconnects OTA
//    ↩️  Back button  → returns to main screen
//
//  IP address shown at bottom for OTA reference
// ─────────────────────────────────────────────────────────

typedef void (*SettingsCloseCb)(const AppConfig& updatedCfg);

class UiSettings {
public:
    // Build the screen once.  cfg = current config to pre-fill fields.
    // onClose is called with updated config when user taps Save or Back.
    void build(const AppConfig& cfg, SettingsCloseCb onClose);

    // Update the displayed IP address (call after OTA connects)
    void setIpAddress(const String& ip);

    lv_obj_t* screen() const { return _screen; }

private:
    lv_obj_t* _screen        = nullptr;
    lv_obj_t* _celsiusSwitch = nullptr;
    lv_obj_t* _brightSlider  = nullptr;
    lv_obj_t* _wifi1Btn      = nullptr;
    lv_obj_t* _wifi2Btn      = nullptr;
    lv_obj_t* _ipLabel       = nullptr;

    SettingsCloseCb _onClose = nullptr;
    AppConfig       _cfg;

    void collectConfig();
    void updateWifiButtons();

    static void onSave(lv_event_t* e);
    static void onBack(lv_event_t* e);
    static void onBrightChange(lv_event_t* e);
    static void onWifi1(lv_event_t* e);
    static void onWifi2(lv_event_t* e);
};
