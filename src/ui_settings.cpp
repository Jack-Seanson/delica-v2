#include "ui_settings.h"
// SCREEN_WIDTH/HEIGHT from board config (no display.h in v2)
#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 480
#endif
#include <string.h>

#define FONT_TITLE  &lv_font_montserrat_20
#define FONT_LABEL  &lv_font_montserrat_16
#define FONT_SMALL  &lv_font_montserrat_14

// Accent colour used for headers / active elements
#define COLOR_ACCENT  lv_color_hex(0x00BFFF)
#define COLOR_BG      lv_color_hex(0x1A1A2E)
#define COLOR_CARD    lv_color_hex(0x16213E)
#define COLOR_TEXT    lv_color_white()

static lv_style_t style_card;
static bool       style_inited = false;

static void init_styles() {
    if (style_inited) return;
    style_inited = true;
    lv_style_init(&style_card);
    lv_style_set_bg_color(&style_card, lv_color_hex(0x16213E));
    lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
    lv_style_set_border_width(&style_card, 0);
    lv_style_set_radius(&style_card, 12);
    lv_style_set_pad_all(&style_card, 12);
}

// Helper: create a section card — NO lv_obj_align
static lv_obj_t* make_card(lv_obj_t* parent, lv_coord_t y, lv_coord_t w, lv_coord_t h) {
    lv_obj_t* card = lv_obj_create(parent);
    lv_obj_set_size(card, w, h);
    lv_obj_set_pos(card, (SCREEN_WIDTH - w) / 2, y);
    lv_obj_add_style(card, &style_card, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    return card;
}

// Helper: label — NO lv_obj_align
static lv_obj_t* make_label(lv_obj_t* parent, const char* text, lv_coord_t x, lv_coord_t y,
                             const lv_font_t* font = &lv_font_montserrat_16) {
    lv_obj_t* lbl = lv_label_create(parent);
    lv_label_set_text(lbl, text);
    lv_obj_set_style_text_font(lbl, font, 0);
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    lv_obj_set_pos(lbl, x, y);
    return lbl;
}

void UiSettings::build(const AppConfig& cfg, SettingsCloseCb onClose) {
    _cfg     = cfg;
    _onClose = onClose;
    init_styles();

    // ── Root screen ───────────────────────────────────────
    _screen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(_screen, COLOR_BG, 0);
    lv_obj_set_style_bg_opa(_screen, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(_screen, 0, 0);

    // Make the screen scrollable so all items are reachable
    lv_obj_set_scroll_dir(_screen, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(_screen, LV_SCROLLBAR_MODE_OFF);

    // ── Title ─────────────────────────────────────────────
    // Centered title: x=0, width=480, y=30
    lv_obj_t* title = lv_label_create(_screen);
    lv_label_set_text(title, LV_SYMBOL_SETTINGS "  SETTINGS");
    lv_obj_set_size(title, 480, LV_SIZE_CONTENT);
    lv_obj_set_pos(title, 0, 30);
    lv_obj_set_style_text_font(title, FONT_TITLE, 0);
    lv_obj_set_style_text_color(title, COLOR_ACCENT, 0);
    lv_obj_set_style_text_letter_space(title, 2, 0);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);

    // ── °F / °C card ──────────────────────────────────────
    lv_obj_t* tempCard = make_card(_screen, 70, 380, 70);
    make_label(tempCard, LV_SYMBOL_REFRESH "  Temperature Unit", 0, 0);

    _celsiusSwitch = lv_switch_create(tempCard);
    lv_obj_set_size(_celsiusSwitch, 60, 30);
    // Right-aligned: card width=380, pad=12 → inner=356; switch at x=286
    lv_obj_set_pos(_celsiusSwitch, 286, 10);
    if (cfg.useCelsius) lv_obj_add_state(_celsiusSwitch, LV_STATE_CHECKED);

    lv_obj_t* fLabel = make_label(tempCard, "\xC2\xB0""F", 200, 10, FONT_SMALL);
    lv_obj_t* cLabel = make_label(tempCard, "\xC2\xB0""C", 240, 10, FONT_SMALL);
    lv_obj_set_style_text_color(fLabel, lv_color_hex(0xAAAAAA), 0);
    lv_obj_set_style_text_color(cLabel, lv_color_hex(0xAAAAAA), 0);

    // ── Brightness card ───────────────────────────────────
    lv_obj_t* brightCard = make_card(_screen, 155, 380, 80);
    make_label(brightCard, LV_SYMBOL_IMAGE "  Brightness", 0, 0);

    _brightSlider = lv_slider_create(brightCard);
    lv_obj_set_size(_brightSlider, 300, 20);
    // Bottom-center of card (h=80, pad=12 → inner=56): y = 56-20-5 = 31, x = (356-300)/2 = 28
    lv_obj_set_pos(_brightSlider, 28, 31);
    lv_slider_set_range(_brightSlider, 10, 100);
    lv_slider_set_value(_brightSlider, cfg.brightness, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(_brightSlider, COLOR_ACCENT, LV_PART_KNOB);
    lv_obj_add_event_cb(_brightSlider, onBrightChange, LV_EVENT_VALUE_CHANGED, this);

    // ── Wi-Fi card ────────────────────────────────────────
    lv_obj_t* wifiCard = make_card(_screen, 250, 380, 140);
    make_label(wifiCard, LV_SYMBOL_WIFI "  Wi-Fi (for OTA updates)", 0, 0);

    make_label(wifiCard, "SSID", 0, 28, FONT_SMALL);
    _ssidField = lv_textarea_create(wifiCard);
    lv_obj_set_size(_ssidField, 340, 36);
    lv_obj_set_pos(_ssidField, 0, 46);
    lv_textarea_set_text(_ssidField, cfg.wifiSSID);
    lv_textarea_set_placeholder_text(_ssidField, "Your Wi-Fi network name");
    lv_textarea_set_one_line(_ssidField, true);
    lv_obj_set_style_text_font(_ssidField, FONT_SMALL, 0);
    lv_obj_set_style_bg_color(_ssidField, lv_color_hex(0x0D1B2A), 0);
    lv_obj_set_style_text_color(_ssidField, lv_color_white(), 0);
    lv_obj_set_style_border_color(_ssidField, COLOR_ACCENT, 0);

    make_label(wifiCard, "Password", 0, 88, FONT_SMALL);
    _passField = lv_textarea_create(wifiCard);
    lv_obj_set_size(_passField, 340, 36);
    lv_obj_set_pos(_passField, 0, 106);
    lv_textarea_set_text(_passField, cfg.wifiPass);
    lv_textarea_set_placeholder_text(_passField, "Password");
    lv_textarea_set_one_line(_passField, true);
    lv_textarea_set_password_mode(_passField, true);
    lv_obj_set_style_text_font(_passField, FONT_SMALL, 0);
    lv_obj_set_style_bg_color(_passField, lv_color_hex(0x0D1B2A), 0);
    lv_obj_set_style_text_color(_passField, lv_color_white(), 0);
    lv_obj_set_style_border_color(_passField, COLOR_ACCENT, 0);

    // ── IP address label ──────────────────────────────────
    // Bottom-center: y=390 (480-90), x=0, width=480
    _ipLabel = lv_label_create(_screen);
    lv_label_set_text(_ipLabel, "OTA: not connected");
    lv_obj_set_size(_ipLabel, 480, LV_SIZE_CONTENT);
    lv_obj_set_pos(_ipLabel, 0, 390);
    lv_obj_set_style_text_font(_ipLabel, FONT_SMALL, 0);
    lv_obj_set_style_text_color(_ipLabel, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_align(_ipLabel, LV_TEXT_ALIGN_CENTER, 0);

    // ── Save button — right of center at bottom ───────────
    // Bottom at y=440 (480-40), centered right: x=240+10=250-80=170
    lv_obj_t* saveBtn = lv_btn_create(_screen);
    lv_obj_set_size(saveBtn, 160, 44);
    lv_obj_set_pos(saveBtn, 250, 430);
    lv_obj_set_style_bg_color(saveBtn, COLOR_ACCENT, 0);
    lv_obj_set_style_bg_color(saveBtn, lv_color_hex(0x0090CC), LV_STATE_PRESSED);
    lv_obj_add_event_cb(saveBtn, onSave, LV_EVENT_CLICKED, this);
    lv_obj_t* saveLbl = lv_label_create(saveBtn);
    lv_label_set_text(saveLbl, LV_SYMBOL_SAVE "  Save");
    lv_obj_set_size(saveLbl, 160, LV_SIZE_CONTENT);
    lv_obj_set_pos(saveLbl, 0, 12);
    lv_obj_set_style_text_font(saveLbl, FONT_LABEL, 0);
    lv_obj_set_style_text_align(saveLbl, LV_TEXT_ALIGN_CENTER, 0);

    // ── Back button — left of center at bottom ────────────
    lv_obj_t* backBtn = lv_btn_create(_screen);
    lv_obj_set_size(backBtn, 160, 44);
    lv_obj_set_pos(backBtn, 70, 430);
    lv_obj_set_style_bg_color(backBtn, lv_color_hex(0x333355), 0);
    lv_obj_set_style_bg_color(backBtn, lv_color_hex(0x222244), LV_STATE_PRESSED);
    lv_obj_add_event_cb(backBtn, onBack, LV_EVENT_CLICKED, this);
    lv_obj_t* backLbl = lv_label_create(backBtn);
    lv_label_set_text(backLbl, LV_SYMBOL_LEFT "  Back");
    lv_obj_set_size(backLbl, 160, LV_SIZE_CONTENT);
    lv_obj_set_pos(backLbl, 0, 12);
    lv_obj_set_style_text_font(backLbl, FONT_LABEL, 0);
    lv_obj_set_style_text_align(backLbl, LV_TEXT_ALIGN_CENTER, 0);
}

void UiSettings::setIpAddress(const String& ip) {
    if (_ipLabel) {
        char buf[64];
        snprintf(buf, sizeof(buf), "OTA: http://%s/update", ip.c_str());
        lv_label_set_text(_ipLabel, buf);
    }
}

void UiSettings::collectConfig() {
    _cfg.useCelsius = lv_obj_has_state(_celsiusSwitch, LV_STATE_CHECKED);
    _cfg.brightness = (uint8_t)lv_slider_get_value(_brightSlider);
    strncpy(_cfg.wifiSSID, lv_textarea_get_text(_ssidField), sizeof(_cfg.wifiSSID) - 1);
    strncpy(_cfg.wifiPass, lv_textarea_get_text(_passField), sizeof(_cfg.wifiPass) - 1);
}

void UiSettings::onSave(lv_event_t* e) {
    UiSettings* self = (UiSettings*)lv_event_get_user_data(e);
    self->collectConfig();
    if (self->_onClose) self->_onClose(self->_cfg);
}

void UiSettings::onBack(lv_event_t* e) {
    UiSettings* self = (UiSettings*)lv_event_get_user_data(e);
    self->collectConfig();
    if (self->_onClose) self->_onClose(self->_cfg);
}

void UiSettings::onBrightChange(lv_event_t* e) {
    UiSettings* self = (UiSettings*)lv_event_get_user_data(e);
    uint8_t val = (uint8_t)lv_slider_get_value(self->_brightSlider);
    self->_cfg.brightness = val;
}
