#include "ui_settings.h"
#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 480
#endif
#include <string.h>

#define FONT_TITLE  &lv_font_montserrat_20
#define FONT_LABEL  &lv_font_montserrat_16
#define FONT_SMALL  &lv_font_montserrat_14

#define COLOR_ACCENT  lv_color_hex(0x00BFFF)
#define COLOR_BG      lv_color_hex(0x1A1A2E)
#define COLOR_CARD    lv_color_hex(0x16213E)

// Friendly display names (set in platformio.ini build flags)
#ifndef WIFI_NAME1
#define WIFI_NAME1 "577 Home"
#endif
#ifndef WIFI_NAME2
#define WIFI_NAME2 "Pixel Hotspot"
#endif
#ifndef WIFI_SSID1
#define WIFI_SSID1 "577EastGrand"
#define WIFI_PASS1 "577EastGrand"
#endif
#ifndef WIFI_SSID2
#define WIFI_SSID2 "Pixel_4720"
#define WIFI_PASS2 "1929Orleans"
#endif

static lv_style_t style_card;
static bool       style_inited = false;

static void init_styles() {
    if (style_inited) return;
    style_inited = true;
    lv_style_init(&style_card);
    lv_style_set_bg_color(&style_card, COLOR_CARD);
    lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
    lv_style_set_border_width(&style_card, 0);
    lv_style_set_radius(&style_card, 12);
    lv_style_set_pad_all(&style_card, 12);
}

static lv_obj_t* make_card(lv_obj_t* parent, lv_coord_t y, lv_coord_t w, lv_coord_t h) {
    lv_obj_t* card = lv_obj_create(parent);
    lv_obj_set_size(card, w, h);
    lv_obj_set_pos(card, (SCREEN_WIDTH - w) / 2, y);
    lv_obj_add_style(card, &style_card, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    return card;
}

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

    _screen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(_screen, COLOR_BG, 0);
    lv_obj_set_style_bg_opa(_screen, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(_screen, 0, 0);
    lv_obj_set_scroll_dir(_screen, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(_screen, LV_SCROLLBAR_MODE_OFF);

    // ── Title ──────────────────────────────────────────
    lv_obj_t* title = lv_label_create(_screen);
    lv_label_set_text(title, LV_SYMBOL_SETTINGS "  SETTINGS");
    lv_obj_set_size(title, 480, LV_SIZE_CONTENT);
    lv_obj_set_pos(title, 0, 30);
    lv_obj_set_style_text_font(title, FONT_TITLE, 0);
    lv_obj_set_style_text_color(title, COLOR_ACCENT, 0);
    lv_obj_set_style_text_letter_space(title, 2, 0);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);

    // ── °F / °C card ───────────────────────────────────
    lv_obj_t* tempCard = make_card(_screen, 70, 380, 70);
    make_label(tempCard, LV_SYMBOL_REFRESH "  Temperature Unit", 0, 0);
    _celsiusSwitch = lv_switch_create(tempCard);
    lv_obj_set_size(_celsiusSwitch, 60, 30);
    lv_obj_set_pos(_celsiusSwitch, 286, 10);
    if (cfg.useCelsius) lv_obj_add_state(_celsiusSwitch, LV_STATE_CHECKED);
    lv_obj_t* fLbl = make_label(tempCard, "\xC2\xB0""F", 200, 10, FONT_SMALL);
    lv_obj_t* cLbl = make_label(tempCard, "\xC2\xB0""C", 240, 10, FONT_SMALL);
    lv_obj_set_style_text_color(fLbl, lv_color_hex(0xAAAAAA), 0);
    lv_obj_set_style_text_color(cLbl, lv_color_hex(0xAAAAAA), 0);

    // ── Brightness card ────────────────────────────────
    lv_obj_t* brightCard = make_card(_screen, 155, 380, 80);
    make_label(brightCard, LV_SYMBOL_IMAGE "  Brightness", 0, 0);
    _brightSlider = lv_slider_create(brightCard);
    lv_obj_set_size(_brightSlider, 300, 20);
    lv_obj_set_pos(_brightSlider, 28, 31);
    lv_slider_set_range(_brightSlider, 10, 100);
    lv_slider_set_value(_brightSlider, cfg.brightness, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(_brightSlider, COLOR_ACCENT, LV_PART_KNOB);
    lv_obj_add_event_cb(_brightSlider, onBrightChange, LV_EVENT_VALUE_CHANGED, this);

    // ── Wi-Fi selector card ────────────────────────────
    // Two tap-to-select buttons; active one highlighted in accent colour.
    lv_obj_t* wifiCard = make_card(_screen, 250, 380, 110);
    make_label(wifiCard, LV_SYMBOL_WIFI "  Wi-Fi Network (for OTA)", 0, 0);

    _wifi1Btn = lv_btn_create(wifiCard);
    lv_obj_set_size(_wifi1Btn, 160, 40);
    lv_obj_set_pos(_wifi1Btn, 0, 28);
    lv_obj_add_event_cb(_wifi1Btn, onWifi1, LV_EVENT_CLICKED, this);
    lv_obj_t* w1lbl = lv_label_create(_wifi1Btn);
    lv_label_set_text(w1lbl, WIFI_NAME1);
    lv_obj_set_style_text_font(w1lbl, FONT_SMALL, 0);
    lv_obj_center(w1lbl);

    _wifi2Btn = lv_btn_create(wifiCard);
    lv_obj_set_size(_wifi2Btn, 160, 40);
    lv_obj_set_pos(_wifi2Btn, 176, 28);
    lv_obj_add_event_cb(_wifi2Btn, onWifi2, LV_EVENT_CLICKED, this);
    lv_obj_t* w2lbl = lv_label_create(_wifi2Btn);
    lv_label_set_text(w2lbl, WIFI_NAME2);
    lv_obj_set_style_text_font(w2lbl, FONT_SMALL, 0);
    lv_obj_center(w2lbl);

    updateWifiButtons();

    // ── IP label ───────────────────────────────────────
    _ipLabel = lv_label_create(_screen);
    lv_label_set_text(_ipLabel, "OTA: not connected");
    lv_obj_set_size(_ipLabel, 480, LV_SIZE_CONTENT);
    lv_obj_set_pos(_ipLabel, 0, 375);
    lv_obj_set_style_text_font(_ipLabel, FONT_SMALL, 0);
    lv_obj_set_style_text_color(_ipLabel, lv_color_hex(0x888888), 0);
    lv_obj_set_style_text_align(_ipLabel, LV_TEXT_ALIGN_CENTER, 0);

    // ── Save button ────────────────────────────────────
    lv_obj_t* saveBtn = lv_btn_create(_screen);
    lv_obj_set_size(saveBtn, 160, 44);
    lv_obj_set_pos(saveBtn, 250, 415);
    lv_obj_set_style_bg_color(saveBtn, COLOR_ACCENT, 0);
    lv_obj_set_style_bg_color(saveBtn, lv_color_hex(0x0090CC), LV_STATE_PRESSED);
    lv_obj_add_event_cb(saveBtn, onSave, LV_EVENT_CLICKED, this);
    lv_obj_t* sLbl = lv_label_create(saveBtn);
    lv_label_set_text(sLbl, LV_SYMBOL_SAVE "  Save");
    lv_obj_set_style_text_font(sLbl, FONT_LABEL, 0);
    lv_obj_center(sLbl);

    // ── Back button ────────────────────────────────────
    lv_obj_t* backBtn = lv_btn_create(_screen);
    lv_obj_set_size(backBtn, 160, 44);
    lv_obj_set_pos(backBtn, 70, 415);
    lv_obj_set_style_bg_color(backBtn, lv_color_hex(0x333355), 0);
    lv_obj_set_style_bg_color(backBtn, lv_color_hex(0x222244), LV_STATE_PRESSED);
    lv_obj_add_event_cb(backBtn, onBack, LV_EVENT_CLICKED, this);
    lv_obj_t* bLbl = lv_label_create(backBtn);
    lv_label_set_text(bLbl, LV_SYMBOL_LEFT "  Back");
    lv_obj_set_style_text_font(bLbl, FONT_LABEL, 0);
    lv_obj_center(bLbl);
}

void UiSettings::setIpAddress(const String& ip) {
    if (!_ipLabel) return;
    char buf[64];
    snprintf(buf, sizeof(buf), "OTA: http://%s", ip.c_str());
    lv_label_set_text(_ipLabel, buf);
}

void UiSettings::updateWifiButtons() {
    // Highlight the active network in accent colour; inactive in dark grey
    bool net1Active = (strcmp(_cfg.wifiSSID, WIFI_SSID1) == 0 || _cfg.wifiSSID[0] == '\0');
    lv_obj_set_style_bg_color(_wifi1Btn, net1Active ? COLOR_ACCENT : lv_color_hex(0x333355), 0);
    lv_obj_set_style_bg_color(_wifi2Btn, net1Active ? lv_color_hex(0x333355) : COLOR_ACCENT, 0);
}

void UiSettings::collectConfig() {
    _cfg.useCelsius = lv_obj_has_state(_celsiusSwitch, LV_STATE_CHECKED);
    _cfg.brightness = (uint8_t)lv_slider_get_value(_brightSlider);
    // wifiSSID/Pass already set by onWifi1/onWifi2 callbacks
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
    self->_cfg.brightness = (uint8_t)lv_slider_get_value(self->_brightSlider);
}

void UiSettings::onWifi1(lv_event_t* e) {
    UiSettings* self = (UiSettings*)lv_event_get_user_data(e);
    strncpy(self->_cfg.wifiSSID, WIFI_SSID1, sizeof(self->_cfg.wifiSSID) - 1);
    strncpy(self->_cfg.wifiPass, WIFI_PASS1, sizeof(self->_cfg.wifiPass) - 1);
    self->updateWifiButtons();
    Serial.printf("[Settings] Wi-Fi → %s\n", WIFI_NAME1);
}

void UiSettings::onWifi2(lv_event_t* e) {
    UiSettings* self = (UiSettings*)lv_event_get_user_data(e);
    strncpy(self->_cfg.wifiSSID, WIFI_SSID2, sizeof(self->_cfg.wifiSSID) - 1);
    strncpy(self->_cfg.wifiPass, WIFI_PASS2, sizeof(self->_cfg.wifiPass) - 1);
    self->updateWifiButtons();
    Serial.printf("[Settings] Wi-Fi → %s\n", WIFI_NAME2);
}
