#pragma once
#include <lvgl.h>
#include "sensors.h"
#include "config.h"

// ─────────────────────────────────────────────────────────
//  Main Display Screen
//
//  Layout (480×480 round display):
//
//    ┌─────────────────────────────────────────────┐
//    │          EXTERNAL                           │
//    │            18°F        ← ext temp label     │
//    │  ┌──────────────────────────────────────┐   │
//    │  │  [background scene image]            │   │
//    │  │                                      │   │
//    │  │    ┌──────────────────────┐          │   │
//    │  │    │   [van pixel art]    │          │   │
//    │  │    │      72°F            │← interior│   │
//    │  │    └──────────────────────┘          │   │
//    │  └──────────────────────────────────────┘   │
//    └─────────────────────────────────────────────┘
//
//  Long-press (1.5s) anywhere → open settings screen
// ─────────────────────────────────────────────────────────

// Weather scene type — determined by external temp
enum WeatherScene {
    SCENE_SNOW   = 0,   // < 32°F
    SCENE_RAIN   = 1,   // 32–50°F
    SCENE_CLOUDY = 2,   // 50–65°F
    SCENE_SUNNY  = 3,   // 65–85°F
    SCENE_HOT    = 4,   // >= 85°F
    SCENE_COUNT  = 5
};

// Callback type — called when user long-presses to open settings
typedef void (*SettingsOpenCb)();

class UiMain {
public:
    // Build the LVGL screen.  Call once after display.begin().
    void build(SettingsOpenCb onSettingsOpen);

    // Refresh temperature labels + swap background if scene changed.
    // Call whenever sensor data or config changes.
    void update(const SensorData& data, const AppConfig& cfg);

    // Return the LVGL screen object (for lv_scr_load)
    lv_obj_t* screen() const { return _screen; }

private:
    lv_obj_t* _screen            = nullptr;
    lv_obj_t* _bgRect            = nullptr;   // solid-colour fallback / image container
    lv_obj_t* _bgImg             = nullptr;   // background scene image (PNG asset)
    lv_obj_t* _vanImg            = nullptr;   // L300 van pixel art (PNG asset)

    lv_obj_t* _extLabel          = nullptr;   // "EXTERNAL" text
    lv_obj_t* _extLabelOutline[8] = {};       // black outline copies of "EXTERNAL"
    lv_obj_t* _extTempLabel      = nullptr;   // external temperature value
    lv_obj_t* _extTempShadow     = nullptr;   // drop-shadow copy of ext temp
    lv_obj_t* _extTempOutline[8] = {};        // black outline copies (8 directions ±1px)

    lv_obj_t* _intLabel          = nullptr;   // "INSIDE" text
    lv_obj_t* _intLabelOutline[8] = {};       // black outline copies of "INSIDE"
    lv_obj_t* _intTempLabel      = nullptr;   // internal temperature value
    lv_obj_t* _intTempShadow     = nullptr;   // drop-shadow copy of int temp
    lv_obj_t* _intTempOutline[8] = {};        // black outline copies (8 directions ±1px)

    lv_obj_t* _errorLabel        = nullptr;   // shown if sensor read fails

    WeatherScene _currentScene = SCENE_COUNT; // force first update

    SettingsOpenCb _onSettingsOpen = nullptr;
    unsigned long  _pressStart     = 0;
    bool           _pressing       = false;

    WeatherScene sceneForTemp(float tempF) const;
    void applyScene(WeatherScene scene);
    void formatTemp(char* buf, size_t len, float tempF, bool celsius);

    // LVGL event callback (static, routes to instance via user_data)
    static void onTouchEvent(lv_event_t* e);
};
