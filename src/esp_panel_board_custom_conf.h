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
// 3-wire SPI control panel pins
// CS is on TCA9554 expander pin index 2 (0-indexed: PIN1=0, PIN2=1, PIN3=2)
#define ESP_PANEL_BOARD_LCD_RGB_SPI_IO_CS               (2)    // expander pin index (0-indexed)
#define ESP_PANEL_BOARD_LCD_RGB_SPI_IO_SCK              (2)    // direct GPIO2
#define ESP_PANEL_BOARD_LCD_RGB_SPI_IO_SDA              (1)    // direct GPIO1
#define ESP_PANEL_BOARD_LCD_RGB_SPI_CS_USE_EXPNADER     (1)    // CS via TCA9554
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

// Waveshare ST7701S vendor init sequence (from Waveshare Arduino demo)
#define ESP_PANEL_BOARD_LCD_VENDOR_INIT_CMD()                                                       \
    {                                                                                                \
        {0xFF, (uint8_t []){0x77,0x01,0x00,0x00,0x10}, 5, 0},                                      \
        {0xC0, (uint8_t []){0x3B,0x00}, 2, 0},                                                      \
        {0xC1, (uint8_t []){0x0B,0x02}, 2, 0},                                                      \
        {0xC2, (uint8_t []){0x07,0x02}, 2, 0},                                                      \
        {0xCC, (uint8_t []){0x10}, 1, 0},                                                           \
        {0xCD, (uint8_t []){0x08}, 1, 0},                                                           \
        {0xB0, (uint8_t []){0x00,0x11,0x16,0x0E,0x11,0x06,0x05,0x09,                               \
                             0x08,0x21,0x06,0x13,0x10,0x29,0x31,0x18}, 16, 0},                     \
        {0xB1, (uint8_t []){0x00,0x11,0x16,0x0E,0x11,0x07,0x05,0x09,                               \
                             0x09,0x21,0x05,0x13,0x11,0x2A,0x31,0x18}, 16, 0},                     \
        {0xFF, (uint8_t []){0x77,0x01,0x00,0x00,0x11}, 5, 0},                                      \
        {0xB0, (uint8_t []){0x6D}, 1, 0},                                                           \
        {0xB1, (uint8_t []){0x37}, 1, 0},                                                           \
        {0xB2, (uint8_t []){0x81}, 1, 0},                                                           \
        {0xB3, (uint8_t []){0x80}, 1, 0},                                                           \
        {0xB5, (uint8_t []){0x43}, 1, 0},                                                           \
        {0xB7, (uint8_t []){0x85}, 1, 0},                                                           \
        {0xB8, (uint8_t []){0x20}, 1, 0},                                                           \
        {0xC1, (uint8_t []){0x78}, 1, 0},                                                           \
        {0xC2, (uint8_t []){0x78}, 1, 0},                                                           \
        {0xD0, (uint8_t []){0x88}, 1, 0},                                                           \
        {0xE0, (uint8_t []){0x00,0x00,0x02}, 3, 0},                                                 \
        {0xE1, (uint8_t []){0x03,0xA0,0x00,0x00,0x04,0xA0,0x00,0x00,0x00,0x20,0x20}, 11, 0},       \
        {0xE2, (uint8_t []){0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                               \
                             0x00,0x00,0x00,0x00,0x00}, 13, 0},                                     \
        {0xE3, (uint8_t []){0x00,0x00,0x11,0x00}, 4, 0},                                           \
        {0xE4, (uint8_t []){0x22,0x00}, 2, 0},                                                      \
        {0xE5, (uint8_t []){0x05,0xEC,0xA0,0xA0,0x07,0xEE,0xA0,0xA0,                               \
                             0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 16, 0},                     \
        {0xE6, (uint8_t []){0x00,0x00,0x11,0x00}, 4, 0},                                           \
        {0xE7, (uint8_t []){0x22,0x00}, 2, 0},                                                      \
        {0xE8, (uint8_t []){0x06,0xED,0xA0,0xA0,0x08,0xEF,0xA0,0xA0,                               \
                             0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 16, 0},                     \
        {0xEB, (uint8_t []){0x00,0x00,0x40,0x40,0x00,0x00,0x00}, 7, 0},                            \
        {0xED, (uint8_t []){0xFF,0xFF,0xFF,0xBA,0x0A,0xBF,0x45,0xFF,                               \
                             0xFF,0x54,0xFB,0xA0,0xAB,0xFF,0xFF,0xFF}, 16, 0},                     \
        {0xEF, (uint8_t []){0x10,0x0D,0x04,0x08,0x3F,0x1F}, 6, 0},                                \
        {0xFF, (uint8_t []){0x77,0x01,0x00,0x00,0x13}, 5, 0},                                      \
        {0xEF, (uint8_t []){0x08}, 1, 0},                                                           \
        {0xFF, (uint8_t []){0x77,0x01,0x00,0x00,0x00}, 5, 0},                                      \
        {0x36, (uint8_t []){0x00}, 1, 0},                                                           \
        {0x3A, (uint8_t []){0x66}, 1, 0},                                                           \
        {0x11, (uint8_t []){0x00}, 0, 120},                                                         \
        {0x20, (uint8_t []){0x00}, 0, 120},                                                         \
        {0x29, (uint8_t []){0x00}, 0, 0},                                                           \
    }

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
#define ESP_PANEL_BOARD_TOUCH_BUS_SKIP_INIT_HOST (1)   // I2C host already initialized by expander

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
#define ESP_PANEL_BOARD_EXPANDER_SKIP_INIT_HOST (0)   // expander initializes I2C host 0
#define ESP_PANEL_BOARD_EXPANDER_I2C_HOST_ID    (0)
#define ESP_PANEL_BOARD_EXPANDER_I2C_CLK_HZ     (400 * 1000)
#define ESP_PANEL_BOARD_EXPANDER_I2C_SCL_PULLUP (1)
#define ESP_PANEL_BOARD_EXPANDER_I2C_SDA_PULLUP (1)
#define ESP_PANEL_BOARD_EXPANDER_I2C_IO_SCL     (7)
#define ESP_PANEL_BOARD_EXPANDER_I2C_IO_SDA     (15)
#define ESP_PANEL_BOARD_EXPANDER_I2C_ADDRESS    (0x20)

// Buzzer (TCA9554 PIN2) is silenced in main.cpp after board->begin()
// using Wire directly to set the output register LOW on pin index 1.

// ── File version ─────────────────────────────────────────────────────────────
#define ESP_PANEL_BOARD_CUSTOM_FILE_VERSION_MAJOR 1
#define ESP_PANEL_BOARD_CUSTOM_FILE_VERSION_MINOR 2
#define ESP_PANEL_BOARD_CUSTOM_FILE_VERSION_PATCH 0

#endif // ESP_PANEL_BOARD_DEFAULT_USE_CUSTOM
