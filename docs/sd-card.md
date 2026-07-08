# Using the micro-SD Card

The badge has **two** SD interfaces. Only one of them works — know which.

## The two slots

| Slot | CS | Status |
|---|---|---|
| **Dedicated micro-SD slot** on the badge PCB | GPIO **10** | ✅ Works — use this one |
| Slot on the back of the display module | GPIO 39 | ❌ Data lines crossed in routing (MISO/MOSI swapped) — unusable with hardware SPI |

Both share the badge SPI bus (SCK 13 / MOSI 11 / MISO 12) with the display, touch, LoRa radio and accessory port. **Park every other chip-select high** (GPIOs 47, 48, 39, 37, 14) before touching the card, or bus traffic will collide.

## Verified configuration

Tested on real hardware with a 16 GB SDHC card ("SS16G"):

```
Name: SS16G
Type: SDHC
Speed: 20.00 MHz (limit: 20.00 MHz)
Size: 15193MB
sd-test: Wrote /sd/badge_test.txt
sd-test: Read back: Hello from the 2024 DEF CON badge!
sd-test: SD TEST: PASS (15193MB card, wrote and read /sd/badge_test.txt)
```

- Bus: SDSPI on `SPI2_HOST`, CS 10, 20 MHz (the ESP-IDF SDSPI default)
- Filesystem: FAT (FAT32 for cards ≥ ~2 GB). A FAT-formatted card mounts as-is.
- Cards that arrive exFAT (common ≥64 GB) or unformatted need a FAT format first — the example does it on the badge via `format_if_mount_failed = true` (**erases the card**), or format it FAT32 on your computer.

## Try it

[examples/sd-test](../examples/sd-test/) is a complete PlatformIO project that mounts the card (formatting if needed), prints the card info, writes `/sd/badge_test.txt`, reads it back, and blinks the green LED slow for PASS / fast for FAIL. Console output on USB at 115200.

```bash
cd examples/sd-test
pio run -t upload --upload-port <PORT>
```

## Minimal code

```c
spi_bus_config_t bus = { .mosi_io_num = 11, .miso_io_num = 12,
                         .sclk_io_num = 13, .quadwp_io_num = -1, .quadhd_io_num = -1 };
spi_bus_initialize(SPI2_HOST, &bus, SDSPI_DEFAULT_DMA);

sdmmc_host_t host = SDSPI_HOST_DEFAULT();
host.slot = SPI2_HOST;
sdspi_device_config_t slot = SDSPI_DEVICE_CONFIG_DEFAULT();
slot.gpio_cs = 10;
slot.host_id = SPI2_HOST;

esp_vfs_fat_sdmmc_mount_config_t cfg = { .format_if_mount_failed = true,
                                         .max_files = 5, .allocation_unit_size = 16*1024 };
sdmmc_card_t *card;
esp_vfs_fat_sdspi_mount("/sd", &host, &slot, &cfg, &card);
// ... standard stdio on /sd/... from here
```

Sharing the bus with a live display (as Doom does) additionally requires serializing access — take a lock around SD transactions so they interleave between display DMA frames.
