/*
 * Custom board configuration for:
 *   Waveshare ESP32-S3-Touch-LCD-2.1
 *   Panel:  ST7701S  480×480  RGB16 + 3-wire SPI init
 *   Touch:  CST816S  I2C (SDA=15, SCL=7, INT=16)
 *   Expander: TCA9554  I2C 0x20 (LCD_RST=PIN1, BUZZER=PIN2, LCD_CS=PIN3)
 *   Backlight: GPIO6 PWM
 */
#pragma once

#define ESP_PANEL_BOARD_DEFAULT_USE_CUSTOM  (1)

#if ESP_PANEL_BOARD_DEFAULT_USE_CUSTOM

// ── General ──────────────────────────────────────────────────────────────────
#define ESP_PANEL_BOARD_NAME    "Waveshare:ESP32-S3-Touch-LCD-2.1"
#define ESP_PANEL_BOARD_WIDTH   (480)
#define ESP_PANEL_BOARD_HEIGHT  (480)

// ── LCD ──────────────────────────────────────────────────────────────────────
#define ESP_PANEL_BOARD_USE_LCD             (1)
#define ESP_PANEL_BOARD_LCD_CONTROLLER      ST7701
#define ESP_PANEL_BOARD_LCD_BUS_TYPE        (ESP_PANEL_BUS_TYPE_RGB)

// RGB bus — uses 3-wire SPI for init commands; CS and RST driven via TCA9554 expander
#define ESP_PANEL_BOARD_LCD_RGB_USE_CONTROL_PANEL       (1)
// 3-wire SPI control panel pins (direct GPIO, not expander)
#define ESP_PANEL_BOARD_LCD_RGB_SPI_IO_CS               (-1)   // CS via expander PIN3 — handled by pre-begin hook
#define ESP_PANEL_BOARD_LCD_RGB_SPI_IO_SCK              (2)
#define ESP_PANEL_BOARD_LCD_RGB_SPI_IO_SDA              (1)
#define ESP_PANEL_BOARD_LCD_RGB_SPI_CS_USE_EXPNADER     (0)
#define ESP_PANEL_BOARD_LCD_RGB_SPI_SCL_USE_EXPNADER    (0)
#define ESP_PANEL_BOARD_LCD_RGB_SPI_SDA_USE_EXPNADER    (0)
#define ESP_PANEL_BOARD_LCD_RGB_SPI_MODE                (0)
#define ESP_PANEL_BOARD_LCD_RGB_SPI_CMD_BYTES           (1)
#define ESP_PANEL_BOARD_LCD_RGB_SPI_PARAM_BYTES         (1)
#define ESP_PANEL_BOARD_LCD_RGB_SPI_USE_DC_BIT          (1)

// RGB refresh panel timing (from Waveshare reference)
#define ESP_PANEL_BOARD_LCD_RGB_CLK_HZ          (18 * 1000 * 1000)
#define ESP_PANEL_BOARD_LCD_RGB_HPW             (8)
#define ESP_PANEL_BOARD_LCD_RGB_HBP             (10)
#define ESP_PANEL_BOARD_LCD_RGB_HFP             (50)
#define ESP_PANEL_BOARD_LCD_RGB_VPW             (3)
#define ESP_PANEL_BOARD_LCD_RGB_VBP             (8)
#define ESP_PANEL_BOARD_LCD_RGB_VFP             (8)
#define ESP_PANEL_BOARD_LCD_RGB_PCLK_ACTIVE_NEG (0)
#define ESP_PANEL_BOARD_LCD_RGB_DATA_WIDTH      (16)
#define ESP_PANEL_BOARD_LCD_RGB_PIXEL_BITS      (ESP_PANEL_LCD_COLOR_BITS_RGB565)
#define ESP_PANEL_BOARD_LCD_RGB_BOUNCE_BUF_SIZE (ESP_PANEL_BOARD_WIDTH * 10)

// RGB data GPIO — 16-bit RGB565 mapping from Waveshare schematic
#define ESP_PANEL_BOARD_LCD_RGB_IO_HSYNC        (38)
#define ESP_PANEL_BOARD_LCD_RGB_IO_VSYNC        (39)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DE           (40)
#define ESP_PANEL_BOARD_LCD_RGB_IO_PCLK         (41)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DISP         (-1)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA0        (5)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA1        (45)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA2        (48)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA3        (47)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA4        (21)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA5        (14)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA6        (13)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA7        (12)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA8        (11)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA9        (10)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA10       (9)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA11       (46)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA12       (3)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA13       (8)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA14       (18)
#define ESP_PANEL_BOARD_LCD_RGB_IO_DATA15       (17)

// Color / transform
#define ESP_PANEL_BOARD_LCD_COLOR_BITS          (ESP_PANEL_LCD_COLOR_BITS_RGB565)
#define ESP_PANEL_BOARD_LCD_COLOR_BGR_ORDER     (0)
#define ESP_PANEL_BOARD_LCD_COLOR_INEVRT_BIT    (0)
#define ESP_PANEL_BOARD_LCD_SWAP_XY             (0)
#define ESP_PANEL_BOARD_LCD_MIRROR_X            (0)
#define ESP_PANEL_BOARD_LCD_MIRROR_Y            (0)
#define ESP_PANEL_BOARD_LCD_GAP_X               (0)
#define ESP_PANEL_BOARD_LCD_GAP_Y               (0)

// RST driven via TCA9554 expander in pre-begin hook — not a direct GPIO
#define ESP_PANEL_BOARD_LCD_RST_IO              (-1)
#define ESP_PANEL_BOARD_LCD_RST_LEVEL           (0)

// ── Touch ─────────────────────────────────────────────────────────────────────
#define ESP_PANEL_BOARD_USE_TOUCH               (1)
#define ESP_PANEL_BOARD_TOUCH_CONTROLLER        CST816S
#define ESP_PANEL_BOARD_TOUCH_BUS_TYPE          (ESP_PANEL_BUS_TYPE_I2C)
#define ESP_PANEL_BOARD_TOUCH_BUS_SKIP_INIT_HOST (0)

#define ESP_PANEL_BOARD_TOUCH_I2C_HOST_ID       (0)
#define ESP_PANEL_BOARD_TOUCH_I2C_CLK_HZ        (400 * 1000)
#define ESP_PANEL_BOARD_TOUCH_I2C_SCL_PULLUP    (1)
#define ESP_PANEL_BOARD_TOUCH_I2C_SDA_PULLUP    (1)
#define ESP_PANEL_BOARD_TOUCH_I2C_IO_SCL        (7)
#define ESP_PANEL_BOARD_TOUCH_I2C_IO_SDA        (15)
#define ESP_PANEL_BOARD_TOUCH_I2C_ADDRESS       (0)   // auto-detect default

#define ESP_PANEL_BOARD_TOUCH_SWAP_XY           (0)
#define ESP_PANEL_BOARD_TOUCH_MIRROR_X          (0)
#define ESP_PANEL_BOARD_TOUCH_MIRROR_Y          (0)
#define ESP_PANEL_BOARD_TOUCH_RST_IO            (-1)
#define ESP_PANEL_BOARD_TOUCH_RST_LEVEL         (0)
#define ESP_PANEL_BOARD_TOUCH_INT_IO            (16)
#define ESP_PANEL_BOARD_TOUCH_INT_LEVEL         (0)   // active LOW

// ── Backlight ─────────────────────────────────────────────────────────────────
#define ESP_PANEL_BOARD_USE_BACKLIGHT           (1)
#define ESP_PANEL_BOARD_BACKLIGHT_TYPE          (ESP_PANEL_BACKLIGHT_TYPE_PWM_LEDC)
#define ESP_PANEL_BOARD_BACKLIGHT_IO            (6)
#define ESP_PANEL_BOARD_BACKLIGHT_ON_LEVEL      (1)
#define ESP_PANEL_BOARD_BACKLIGHT_PWM_FREQ_HZ       (25000)
#define ESP_PANEL_BOARD_BACKLIGHT_PWM_DUTY_RESOLUTION (10)
#define ESP_PANEL_BOARD_BACKLIGHT_IDLE_OFF      (0)

// ── IO Expander (TCA9554 at 0x20) ─────────────────────────────────────────────
// Used to drive LCD_RST (PIN1), BUZZER (PIN2), LCD_CS (PIN3)
#define ESP_PANEL_BOARD_USE_EXPANDER            (1)
#define ESP_PANEL_BOARD_EXPANDER_CHIP           TCA95XX_8BIT
#define ESP_PANEL_BOARD_EXPANDER_SKIP_INIT_HOST (1)   // share I2C host with touch
#define ESP_PANEL_BOARD_EXPANDER_I2C_HOST_ID    (0)
#define ESP_PANEL_BOARD_EXPANDER_I2C_ADDRESS    (0x20)

// ── Pre-begin: silence buzzer via TCA9554 before LCD init ────────────────────
// The expander is already initialized by the board driver at this point.
// We set PIN2 (BUZZER) LOW and ensure PIN1 (RST) and PIN3 (CS) are HIGH.
// This replaces the manual tca_init() from the old display.cpp.
#define ESP_PANEL_BOARD_EXPANDER_POST_BEGIN_FUNCTION(p)         \
    {                                                            \
        auto board = static_cast<esp_panel::board::Board *>(p); \
        auto expander = board->getExpander();                    \
        if (expander) {                                          \
            expander->pinMode(0, OUTPUT);  /* PIN1: LCD_RST */  \
            expander->pinMode(1, OUTPUT);  /* PIN2: BUZZER  */  \
            expander->pinMode(2, OUTPUT);  /* PIN3: LCD_CS  */  \
            expander->digitalWrite(0, HIGH); /* RST=HIGH */     \
            expander->digitalWrite(1, LOW);  /* BUZZER=OFF */   \
            expander->digitalWrite(2, HIGH); /* CS=HIGH */      \
        }                                                        \
        return true;                                             \
    }

// ── File version ─────────────────────────────────────────────────────────────
#define ESP_PANEL_BOARD_CUSTOM_FILE_VERSION_MAJOR 1
#define ESP_PANEL_BOARD_CUSTOM_FILE_VERSION_MINOR 2
#define ESP_PANEL_BOARD_CUSTOM_FILE_VERSION_PATCH 0

#endif // ESP_PANEL_BOARD_DEFAULT_USE_CUSTOM
