#ifndef Pins_Arduino_h
#define Pins_Arduino_h
#include "variant.h"
#include <stdint.h>
#define USB_VID 0x303a
#define USB_PID 0x1001
static const uint8_t SDA = 35;
static const uint8_t SCL = 36;
static const uint8_t SS = 48;
static const uint8_t MOSI = 11;
static const uint8_t MISO = 12;
static const uint8_t SCK = 13;
#define SPI_MOSI (11)
#define SPI_SCK (13)
#define SPI_MISO (12)
#define SPI_CS (48)
#define LED_BUILTIN (2)
#endif /* Pins_Arduino_h */
