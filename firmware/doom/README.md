# DOOM

Yes, really. PrBoom 2.5.0 running the shareware **DOOM** (Knee-Deep in the Dead) natively on the badge — 320×240 fullscreen, all game logic, attract demos, saves-free arcade style. The WAD is memory-mapped straight out of flash so the whole game fits in the S3's 2 MB PSRAM.

## Flash it

```bash
esptool --chip esp32s3 --port <PORT> write-flash 0x0 doom-audio.factory.bin
```

| File | Sound |
|---|---|
| `doom-audio.factory.bin` | Sound effects through the piezo buzzer (loud-ish, crunchy, gloriously PC-speaker). Three self-test chirps at boot. |
| `doom-silent.factory.bin` | Mute. Same game otherwise. |

Music is not implemented in either build (there's no MIDI synth in this port — yet).

## Controls

| Input | Action |
|---|---|
| D-pad | Move forward/back, turn left/right |
| **A** (SW8) | Fire — also Enter in menus |
| **B** (SW7) | Use / open doors — Space in menus |
| **A+B together** | Escape (open/close menu) |
| **BOOT** (SW2, top of badge) | Switch weapon (best-available toggle) |

At the title screen: **A+B** → menu → **A** to start New Game.

## How it works / source

Port derived from [cocus/esp32-ili9341-doom-lvgl](https://github.com/cocus/esp32-ili9341-doom-lvgl) (itself from [espressif/esp32-doom](https://github.com/espressif/esp32-doom)), retargeted to the badge:

- Badge pin map, all six face buttons + BOOT wired to Doom keys
- IWAD (`doom1.wad`, shareware — freely redistributable) and PrBoom's data WAD live in dedicated flash partitions, memory-mapped, lumps read in place
- Zone allocator backed by PSRAM with cache eviction — fits the 2 MB
- 8-bit palette renderer → RGB565 over 40 MHz SPI
- Audio build: 1-bit sigma-delta on the buzzer GPIO, 22 kHz sample timer, heavy gain+clip (a piezo demands it)

Flash layout inside the factory image: bootloader `0x0`, partition table `0x8000`, app `0x10000`, `doom1.wad` `0x210000`, `prboom-plus.wad` `0x630000`.
