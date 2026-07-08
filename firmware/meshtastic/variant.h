#pragma once

// --- Pin Definitions (Matching 2024 Def Con Badge v1) ---
#define PIN_LED_DEBUG 2      // green LED
#define PIN_CS_ACCESSORY -1  // not used

// --- SPI Pins (Shared by Radio and TFT Display) ---
#define PIN_SPI_SCK 13
#define PIN_SPI_MISO 12
#define PIN_SPI_MOSI 11
#define PIN_SPI_CS 48

// --- I2C ---
// Not used, but let's define defaults to avoid compilation warnings
#define PIN_I2C0_SCL 36
#define PIN_I2C0_SDA 35
#define I2C_SDA PIN_I2C0_SDA
#define I2C_SCL PIN_I2C0_SCL

// --- Heartbeat Status LED ---
#define LED_POWER PIN_LED_DEBUG
#define LED_STATE_ON 1

// --- Performance & Feature Defaults ---
#define AMBIENT_LIGHTING_AUTO_START

// --- RGB NeoPixel (Ambient Lighting) ---
#define HAS_NEOPIXEL 1
#define NEOPIXEL_COUNT 10
#define NEOPIXEL_DATA 17
#define NEOPIXEL_TYPE (NEO_GRB + NEO_KHZ800)
#define ENABLE_AMBIENT_LIGHTING

// --- Radio Setup (RFM95W) ---
#define USE_RF95
#define LORA_SCK PIN_SPI_SCK
#define LORA_MISO PIN_SPI_MISO
#define LORA_MOSI PIN_SPI_MOSI
#define LORA_CS PIN_SPI_CS
#define LORA_RESET 38
#define LORA_DIO0 21
#define LORA_DIO1 RADIOLIB_NC
#define LORA_DIO2 RADIOLIB_NC

// --- Buzzer ---
#define PIN_BUZZER 9
#define USE_PIN_BUZZER

// --- Navigation Buttons (using Trackball Input Broker) ---
#define HAS_TRACKBALL 1
#define TB_LEFT 3       // SW3
#define TB_UP 4         // SW4
#define TB_DOWN 5       // SW5
#define TB_RIGHT 6      // SW6
#define TB_PRESS 8      // SW8 (Button A)
#define TB_DIRECTION FALLING

#define BUTTON_PIN 8    // Map main user button to SW8 (Button A)
#define BUTTON_NEED_PULLUP

// --- ILI9341 TFT Display Config ---
#define ILI9341_DRIVER
#define USE_TFTDISPLAY 1
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define SCREEN_ROTATE
#define TFT_OFFSET_X 0
#define TFT_OFFSET_Y 0
#define TFT_BUSY -1
#define ILI9341_SPI_HOST SPI2_HOST
#define TFT_SCLK PIN_SPI_SCK
#define TFT_MOSI PIN_SPI_MOSI
#define TFT_MISO PIN_SPI_MISO
#define TFT_CS 47       // CS_TFT_SCREEN
#define TFT_DC 40       // DC_TFT_SCREEN
#define TFT_RST 41      // RESET_TFT_SCREEN
#define TFT_BL -1       // Backlight hardwired to 3.3V
#define SPI_FREQUENCY 40000000
#define SPI_READ_FREQUENCY 16000000
