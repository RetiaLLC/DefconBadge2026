// SD card smoke test for the 2024 DEF CON badge (dedicated micro-SD slot).
//
// The badge has TWO SD interfaces: this dedicated slot (CS GPIO10) and the
// unpopulated-by-default slot on the back of the display module (CS GPIO39).
// Both share the badge-wide SPI bus (SCK 13 / MOSI 11 / MISO 12) with the
// display, touch controller, LoRa radio and accessory port — so every other
// chip-select must be parked high before talking to the card.
//
// Output goes to the USB serial console. The green debug LED (GPIO2) blinks
// slow on success, fast on failure.

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "driver/gpio.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"

static const char *TAG = "sd-test";

#define PIN_SCK   13
#define PIN_MOSI  11
#define PIN_MISO  12
#define PIN_SD_CS 10   // dedicated micro-SD slot
#define PIN_LED    2

// Everything else on the shared bus, parked high (deselected)
static const gpio_num_t cs_park[] = {
    GPIO_NUM_47, // TFT display
    GPIO_NUM_48, // LoRa RFM95W
    GPIO_NUM_39, // display-module SD slot
    GPIO_NUM_37, // accessory connector
    GPIO_NUM_14, // XPT2046 touch
};

static void blink_forever(int on_ms, int off_ms)
{
    gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);
    while (1) {
        gpio_set_level(PIN_LED, 1);
        vTaskDelay(pdMS_TO_TICKS(on_ms));
        gpio_set_level(PIN_LED, 0);
        vTaskDelay(pdMS_TO_TICKS(off_ms));
    }
}

void app_main(void)
{
    for (int i = 0; i < sizeof(cs_park)/sizeof(cs_park[0]); i++) {
        gpio_reset_pin(cs_park[i]);
        gpio_set_direction(cs_park[i], GPIO_MODE_OUTPUT);
        gpio_set_level(cs_park[i], 1);
    }

    ESP_LOGI(TAG, "Initializing SPI bus (SCK=%d MOSI=%d MISO=%d)", PIN_SCK, PIN_MOSI, PIN_MISO);
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &bus_cfg, SDSPI_DEFAULT_DMA));

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SPI2_HOST;

    sdspi_device_config_t slot_cfg = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_cfg.gpio_cs = PIN_SD_CS;
    slot_cfg.host_id = SPI2_HOST;

    // format_if_mount_failed: a brand-new or exFAT card (most cards >32GB,
    // and many 16GB cards ship exFAT/FAT32-with-odd-geometry) gets formatted
    // FAT here. THIS ERASES THE CARD.
    esp_vfs_fat_sdmmc_mount_config_t mount_cfg = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    sdmmc_card_t *card;
    ESP_LOGI(TAG, "Mounting SD card (CS=%d)...", PIN_SD_CS);
    esp_err_t ret = esp_vfs_fat_sdspi_mount("/sd", &host, &slot_cfg, &mount_cfg, &card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Mount failed: %s (0x%x)", esp_err_to_name(ret), ret);
        ESP_LOGE(TAG, "SD TEST: FAIL — no card detected or card unusable");
        blink_forever(100, 100);
    }

    ESP_LOGI(TAG, "Card mounted:");
    sdmmc_card_print_info(stdout, card);

    // Write
    const char *path = "/sd/badge_test.txt";
    FILE *f = fopen(path, "w");
    if (!f) {
        ESP_LOGE(TAG, "SD TEST: FAIL — could not open %s for writing", path);
        blink_forever(100, 100);
    }
    fprintf(f, "Hello from the 2024 DEF CON badge!\nSD slot CS=GPIO%d works.\n", PIN_SD_CS);
    fclose(f);
    ESP_LOGI(TAG, "Wrote %s", path);

    // Read back
    char buf[128] = {0};
    f = fopen(path, "r");
    if (!f || !fgets(buf, sizeof(buf), f)) {
        ESP_LOGE(TAG, "SD TEST: FAIL — could not read back %s", path);
        blink_forever(100, 100);
    }
    fclose(f);
    ESP_LOGI(TAG, "Read back: %s", buf);

    ESP_LOGI(TAG, "SD TEST: PASS (%lluMB card, wrote and read %s)",
             ((uint64_t)card->csd.capacity) * card->csd.sector_size / (1024 * 1024), path);
    blink_forever(800, 800);
}
