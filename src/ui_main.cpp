#include "ui_main.h"
#include "config.h"
#include "img_bg_frozen.h"
#include "img_bg_cold.h"
#include "img_bg_perfect.h"
#include "img_bg_hot.h"
#include "img_van.h"
#include <stdio.h>

// ─────────────────────────────────────────────────────────
//  Scene background images
//  SCENE_SNOW   (<32°F)  → frozen/winter
//  SCENE_RAIN   (<55°F)  → cold
//  SCENE_CLOUDY (<70°F)  → cold (same)
//  SCENE_SUNNY  (<85°F)  → perfect
//  SCENE_HOT    (≥85°F)  → hot
// ─────────────────────────────────────────────────────────
static const lv_img_dsc_t* BG_IMAGES[SCENE_COUNT] = {
    &img_bg_frozen,   // SCENE_SNOW
    &img_bg_cold,     // SCENE_RAIN
    &img_bg_cold,     // SCENE_CLOUDY
    &img_bg_perfect,  // SCENE_SUNNY
    &img_bg_hot,      // SCENE_HOT
};

// ─────────────────────────────────────────────────────────
//  Outline offsets — 8 directions at ±1px
// ─────────────────────────────────────────────────────────
static const lv_coord_t OUTLINE_DX[8] = { -1,  0,  1, -1,  1, -1,  0,  1 };
static const lv_coord_t OUTLINE_DY[8] = { -1, -1, -1,  0,  0,  1,  1,  1 };

// ─────────────────────────────────────────────────────────
//  Layout constants  (480 × 480 screen)
//
//  Top band  (y  0 – 145):  OUTSIDE label + big ext temp
//  Middle    (y 145 – 420): Van image (280 × 280, centered)
//  Bottom    (y 360 – 480): INSIDE label + int temp (same size as old ext)
// ─────────────────────────────────────────────────────────
static constexpr int VAN_W  = 280;
static constexpr int VAN_H  = 280;
static constexpr int VAN_X  = (480 - VAN_W) / 2;   // 100
static constexpr int VAN_Y  = 160;                  // shifted down slightly for larger top text

// Ext label/temp sit in the upper sky, centred
static constexpr int EXT_LABEL_Y    = 95;    // was 110 — shifted up for taller label font
static constexpr int EXT_TEMP_Y     = 122;   // was 135 — sits below bigger label

// Interior temp — same font size as old exterior, fits above bottom of van
static constexpr int INT_LABEL_Y    = 268;
static constexpr int INT_TEMP_Y     = 290;
static constexpr int ERROR_Y        = 455;

// ─────────────────────────────────────────────────────────
//  Build the main screen LVGL hierarchy
//
//  Two-pass pattern: create ALL objects first (no styles),
//  then apply all styles — avoids LVGL 8 cascade segfault.
// ─────────────────────────────────────────────────────────
void UiMain::build(SettingsOpenCb onSettingsOpen) {
    _onSettingsOpen = onSettingsOpen;

    // ══════════════════════════════════════════════════════
    //  PASS 1: Create all objects — geometry + text only
    // ══════════════════════════════════════════════════════

    _screen = lv_obj_create(nullptr);
    lv_obj_clear_flag(_screen, LV_OBJ_FLAG_SCROLLABLE);

    // ── Background image ────────────────────────────────
    _bgImg = lv_img_create(_screen);
    lv_img_set_src(_bgImg, &img_bg_frozen);
    lv_obj_set_pos(_bgImg, 0, 0);
    lv_obj_set_size(_bgImg, 480, 480);
    lv_obj_clear_flag(_bgImg, LV_OBJ_FLAG_CLICKABLE);

    // ── "OUTSIDE" label — 8 outline copies + foreground ──
    for (int i = 0; i < 8; i++) {
        _extLabelOutline[i] = lv_label_create(_screen);
        lv_label_set_long_mode(_extLabelOutline[i], LV_LABEL_LONG_CLIP);
        lv_label_set_text(_extLabelOutline[i], "OUTSIDE");
        lv_obj_set_size(_extLabelOutline[i], 480, LV_SIZE_CONTENT);
        lv_obj_set_pos(_extLabelOutline[i], OUTLINE_DX[i], EXT_LABEL_Y + OUTLINE_DY[i]);
    }
    _extLabel = lv_label_create(_screen);
    lv_label_set_long_mode(_extLabel, LV_LABEL_LONG_CLIP);
    lv_label_set_text(_extLabel, "OUTSIDE");
    lv_obj_set_size(_extLabel, 480, LV_SIZE_CONTENT);
    lv_obj_set_pos(_extLabel, 0, EXT_LABEL_Y);

    // ── External temp — 8 outline + 1 shadow + 1 main ──
    for (int i = 0; i < 8; i++) {
        _extTempOutline[i] = lv_label_create(_screen);
        lv_label_set_long_mode(_extTempOutline[i], LV_LABEL_LONG_CLIP);
        lv_label_set_text(_extTempOutline[i], "--");
        lv_obj_set_size(_extTempOutline[i], 480, LV_SIZE_CONTENT);
        lv_obj_set_pos(_extTempOutline[i], OUTLINE_DX[i], EXT_TEMP_Y + OUTLINE_DY[i]);
    }
    _extTempShadow = lv_label_create(_screen);
    lv_label_set_long_mode(_extTempShadow, LV_LABEL_LONG_CLIP);
    lv_label_set_text(_extTempShadow, "--");
    lv_obj_set_size(_extTempShadow, 480, LV_SIZE_CONTENT);
    lv_obj_set_pos(_extTempShadow, 3, EXT_TEMP_Y + 3);

    _extTempLabel = lv_label_create(_screen);
    lv_label_set_long_mode(_extTempLabel, LV_LABEL_LONG_CLIP);
    lv_label_set_text(_extTempLabel, "--");
    lv_obj_set_size(_extTempLabel, 480, LV_SIZE_CONTENT);
    lv_obj_set_pos(_extTempLabel, 0, EXT_TEMP_Y);

    // ── Van image ────────────────────────────────────────
    _vanImg = lv_img_create(_screen);
    lv_img_set_src(_vanImg, &img_van);
    lv_obj_set_pos(_vanImg, VAN_X, VAN_Y);
    lv_obj_clear_flag(_vanImg, LV_OBJ_FLAG_CLICKABLE);

    // ── "INSIDE" label — 8 outline copies + foreground ──
    for (int i = 0; i < 8; i++) {
        _intLabelOutline[i] = lv_label_create(_screen);
        lv_label_set_long_mode(_intLabelOutline[i], LV_LABEL_LONG_CLIP);
        lv_label_set_text(_intLabelOutline[i], "INSIDE");
        lv_obj_set_size(_intLabelOutline[i], 480, LV_SIZE_CONTENT);
        lv_obj_set_pos(_intLabelOutline[i], OUTLINE_DX[i], INT_LABEL_Y + OUTLINE_DY[i]);
    }
    _intLabel = lv_label_create(_screen);
    lv_label_set_long_mode(_intLabel, LV_LABEL_LONG_CLIP);
    lv_label_set_text(_intLabel, "INSIDE");
    lv_obj_set_size(_intLabel, 480, LV_SIZE_CONTENT);
    lv_obj_set_pos(_intLabel, 0, INT_LABEL_Y);

    // ── Interior temp — 8 outline + 1 shadow + 1 main ──
    for (int i = 0; i < 8; i++) {
        _intTempOutline[i] = lv_label_create(_screen);
        lv_label_set_long_mode(_intTempOutline[i], LV_LABEL_LONG_CLIP);
        lv_label_set_text(_intTempOutline[i], "--");
        lv_obj_set_size(_intTempOutline[i], 480, LV_SIZE_CONTENT);
        lv_obj_set_pos(_intTempOutline[i], OUTLINE_DX[i], INT_TEMP_Y + OUTLINE_DY[i]);
    }
    _intTempShadow = lv_label_create(_screen);
    lv_label_set_long_mode(_intTempShadow, LV_LABEL_LONG_CLIP);
    lv_label_set_text(_intTempShadow, "--");
    lv_obj_set_size(_intTempShadow, 480, LV_SIZE_CONTENT);
    lv_obj_set_pos(_intTempShadow, 3, INT_TEMP_Y + 3);

    _intTempLabel = lv_label_create(_screen);
    lv_label_set_long_mode(_intTempLabel, LV_LABEL_LONG_CLIP);
    lv_label_set_text(_intTempLabel, "--");
    lv_obj_set_size(_intTempLabel, 480, LV_SIZE_CONTENT);
    lv_obj_set_pos(_intTempLabel, 0, INT_TEMP_Y);

    // ── Error label ──────────────────────────────────────
    _errorLabel = lv_label_create(_screen);
    lv_label_set_long_mode(_errorLabel, LV_LABEL_LONG_CLIP);
    lv_label_set_text(_errorLabel, "SENSOR ERROR");
    lv_obj_set_size(_errorLabel, 480, LV_SIZE_CONTENT);
    lv_obj_set_pos(_errorLabel, 0, ERROR_Y);
    lv_obj_add_flag(_errorLabel, LV_OBJ_FLAG_HIDDEN);

    // Touch is handled by direct INT pin polling in main.cpp (no LVGL indev)
    // so we do NOT register a touch event handler here.

    // ══════════════════════════════════════════════════════
    //  PASS 2: Apply styles — all objects fully created now
    // ══════════════════════════════════════════════════════

    // Screen background
    lv_obj_set_style_bg_color(_screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(_screen, LV_OPA_COVER, 0);

    // "OUTSIDE" label outlines (black) — font_28 (~30% larger than old font_20)
    for (int i = 0; i < 8; i++) {
        lv_obj_set_style_text_font(_extLabelOutline[i], &lv_font_montserrat_28, 0);
        lv_obj_set_style_text_align(_extLabelOutline[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(_extLabelOutline[i], lv_color_black(), 0);
        lv_obj_set_style_text_letter_space(_extLabelOutline[i], 4, 0);
    }
    // "OUTSIDE" label foreground (white)
    lv_obj_set_style_text_font(_extLabel, &lv_font_montserrat_28, 0);
    lv_obj_set_style_text_align(_extLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(_extLabel, lv_color_white(), 0);
    lv_obj_set_style_text_letter_space(_extLabel, 4, 0);

    // External temp outlines (black)
    for (int i = 0; i < 8; i++) {
        lv_obj_set_style_text_font(_extTempOutline[i], &lv_font_montserrat_48, 0);
        lv_obj_set_style_text_align(_extTempOutline[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(_extTempOutline[i], lv_color_black(), 0);
    }
    // External temp shadow
    lv_obj_set_style_text_font(_extTempShadow, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_align(_extTempShadow, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(_extTempShadow, lv_color_black(), 0);
    lv_obj_set_style_text_opa(_extTempShadow, LV_OPA_50, 0);
    // External temp main (white)
    lv_obj_set_style_text_font(_extTempLabel, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_align(_extTempLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(_extTempLabel, lv_color_white(), 0);

    // "INSIDE" label outlines (black) — font_20 (matches old OUTSIDE label size)
    for (int i = 0; i < 8; i++) {
        lv_obj_set_style_text_font(_intLabelOutline[i], &lv_font_montserrat_20, 0);
        lv_obj_set_style_text_align(_intLabelOutline[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(_intLabelOutline[i], lv_color_black(), 0);
        lv_obj_set_style_text_letter_space(_intLabelOutline[i], 2, 0);
    }
    // "INSIDE" label foreground (light blue)
    lv_obj_set_style_text_font(_intLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_align(_intLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(_intLabel, lv_color_hex(0xAAEEFF), 0);
    lv_obj_set_style_text_letter_space(_intLabel, 2, 0);

    // Interior temp outlines (black) — font_48 (matches OUTSIDE temp size)
    for (int i = 0; i < 8; i++) {
        lv_obj_set_style_text_font(_intTempOutline[i], &lv_font_montserrat_48, 0);
        lv_obj_set_style_text_align(_intTempOutline[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_color(_intTempOutline[i], lv_color_black(), 0);
    }
    // Interior temp shadow
    lv_obj_set_style_text_font(_intTempShadow, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_align(_intTempShadow, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(_intTempShadow, lv_color_black(), 0);
    lv_obj_set_style_text_opa(_intTempShadow, LV_OPA_50, 0);
    // Interior temp main (light blue)
    lv_obj_set_style_text_font(_intTempLabel, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_align(_intTempLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(_intTempLabel, lv_color_hex(0xAAEEFF), 0);

    // Error label
    lv_obj_set_style_text_font(_errorLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(_errorLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(_errorLabel, lv_color_hex(0xFF4444), 0);
}

// ─────────────────────────────────────────────────────────
//  Update labels from fresh sensor data
// ─────────────────────────────────────────────────────────
void UiMain::update(const SensorData& data, const AppConfig& cfg) {
    char buf[16];

    if (data.externalValid) {
        formatTemp(buf, sizeof(buf), data.externalTempF, cfg.useCelsius);
        lv_label_set_text(_extTempLabel,  buf);
        lv_label_set_text(_extTempShadow, buf);
        for (int i = 0; i < 8; i++) lv_label_set_text(_extTempOutline[i], buf);

        WeatherScene scene = sceneForTemp(data.externalTempF);
        if (scene != _currentScene) applyScene(scene);
    } else {
        lv_label_set_text(_extTempLabel,  "--");
        lv_label_set_text(_extTempShadow, "--");
        for (int i = 0; i < 8; i++) lv_label_set_text(_extTempOutline[i], "--");
        // No BLE data — neutral (perfect/sunny) background
        if (_currentScene != SCENE_SUNNY) applyScene(SCENE_SUNNY);
    }

    if (data.internalValid) {
        formatTemp(buf, sizeof(buf), data.internalTempF, cfg.useCelsius);
        lv_label_set_text(_intTempLabel,  buf);
        lv_label_set_text(_intTempShadow, buf);
        for (int i = 0; i < 8; i++) lv_label_set_text(_intTempOutline[i], buf);
    } else {
        lv_label_set_text(_intTempLabel,  "--");
        lv_label_set_text(_intTempShadow, "--");
        for (int i = 0; i < 8; i++) lv_label_set_text(_intTempOutline[i], "--");
    }

    if (!data.internalValid || !data.externalValid) {
        lv_obj_clear_flag(_errorLabel, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(_errorLabel, LV_OBJ_FLAG_HIDDEN);
    }
}

// ─────────────────────────────────────────────────────────
//  Private helpers
// ─────────────────────────────────────────────────────────
WeatherScene UiMain::sceneForTemp(float tempF) const {
    if (tempF <  TEMP_SNOW_BELOW)   return SCENE_SNOW;
    if (tempF <  TEMP_RAIN_BELOW)   return SCENE_RAIN;
    if (tempF <  TEMP_CLOUDY_BELOW) return SCENE_CLOUDY;
    if (tempF <  TEMP_SUNNY_BELOW)  return SCENE_SUNNY;
    return SCENE_HOT;
}

void UiMain::applyScene(WeatherScene scene) {
    _currentScene = scene;
    if (_bgImg) lv_img_set_src(_bgImg, BG_IMAGES[scene]);
}

void UiMain::formatTemp(char* buf, size_t len, float tempF, bool celsius) {
    if (celsius) {
        float c = SensorManager::toC(tempF);
        snprintf(buf, len, "%.0f\xC2\xB0""C", c);
    } else {
        snprintf(buf, len, "%.0f\xC2\xB0""F", tempF);
    }
}

// ─────────────────────────────────────────────────────────
//  Touch event — long-press for settings
// ─────────────────────────────────────────────────────────
void UiMain::onTouchEvent(lv_event_t* e) {
    UiMain* self = (UiMain*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_PRESSED) {
        self->_pressing   = true;
        self->_pressStart = millis();
    } else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        self->_pressing = false;
    } else if (code == LV_EVENT_LONG_PRESSED) {
        if (self->_pressing && (millis() - self->_pressStart >= LONGPRESS_MS)) {
            self->_pressing = false;
            if (self->_onSettingsOpen) self->_onSettingsOpen();
        }
    }
}
