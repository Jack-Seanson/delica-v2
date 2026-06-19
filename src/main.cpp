/*
 * Delica L300 Temperature Display v2
 * Waveshare ESP32-S3-Touch-LCD-2.1
 *
 * Stage 1: Hello World — verify display + touch work before porting full UI.
 */

#include <Arduino.h>
#include <Wire.h>
#include <esp_display_panel.hpp>
#include <lvgl.h>
#include "lvgl_v8_port.h"

using namespace esp_panel::drivers;
using namespace esp_panel::board;

static Board *board = nullptr;
static lv_obj_t *touch_label = nullptr;

void setup()
{
    Serial.begin(115200);
    delay(300);
    Serial.println("\n========================================");
    Serial.println("  Delica L300 Temperature Display v2");
    Serial.println("========================================");

    // ── Init board (LCD + touch + backlight via ESP32_Display_Panel) ──────────
    Serial.println("[Board] Initializing...");
    board = new Board();
    board->init();

    // Bounce buffer for RGB LCD — prevents screen drift on ESP32-S3
    auto lcd = board->getLCD();
    lcd->configFrameBufferNumber(2);
    auto lcd_bus = lcd->getBus();
    if (lcd_bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
        static_cast<BusRGB *>(lcd_bus)->configRGB_BounceBufferSize(lcd->getFrameWidth() * 10);
    }

    assert(board->begin());
    Serial.println("[Board] Ready");

    // ── Init LVGL + touch ─────────────────────────────────────────────────────
    Serial.println("[LVGL] Initializing...");
    lvgl_port_init(board->getLCD(), board->getTouch());
    Serial.println("[LVGL] Ready");

    // ── Build simple test UI ──────────────────────────────────────────────────
    lvgl_port_lock(-1);

    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x1A1A2E), 0);

    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "Delica L300 v2");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_30, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xE0E0E0), 0);
    lv_obj_align(title, LV_ALIGN_CENTER, 0, -60);

    lv_obj_t *sub = lv_label_create(scr);
    lv_label_set_text(sub, "ESP32_Display_Panel + LVGL 8.4");
    lv_obj_set_style_text_font(sub, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(sub, lv_color_hex(0x888888), 0);
    lv_obj_align_to(sub, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);

    touch_label = lv_label_create(scr);
    lv_label_set_text(touch_label, "Tap the screen...");
    lv_obj_set_style_text_font(touch_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(touch_label, lv_color_hex(0x00BCD4), 0);
    lv_obj_align(touch_label, LV_ALIGN_CENTER, 0, 60);

    // Touch event on screen
    lv_obj_add_flag(scr, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(scr, [](lv_event_t *e) {
        lv_indev_t *indev = lv_indev_get_act();
        lv_point_t p;
        lv_indev_get_point(indev, &p);
        char buf[48];
        snprintf(buf, sizeof(buf), "Touch: x=%d  y=%d", p.x, p.y);
        lv_label_set_text(touch_label, buf);
        Serial.printf("[Touch] x=%d y=%d\n", p.x, p.y);
    }, LV_EVENT_CLICKED, nullptr);

    lvgl_port_unlock();

    Serial.println("[App] Setup complete");
}

void loop()
{
    // LVGL runs in its own FreeRTOS task — loop just handles non-LVGL work
    delay(100);
}
