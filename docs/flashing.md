# Flashing Guide

Everything here uses the badge's **USB-C port** — no programmer needed. The ESP32-S3's native USB shows up as a serial port when you plug it in.

## 1. Install esptool

```bash
pip install esptool
```

## 2. Find the port

| OS | Port looks like |
|---|---|
| macOS | `/dev/cu.usbmodem*` |
| Linux | `/dev/ttyACM0` |
| Windows | `COM5` (check Device Manager) |

> A badge running Meshtastic enumerates with its MAC address in the port name (e.g. `usbmodem744DBD216494`); a badge in the ROM bootloader or running most other firmware shows a generic name.

## 3. Flash a factory image

Every `.factory.bin` in this repo is a **complete merged image** (bootloader + partition table + app + data). One command, always at offset 0:

```bash
esptool --chip esp32s3 --port /dev/cu.usbmodem1101 write-flash 0x0 doom-audio.factory.bin
```

The badge resets and boots the new firmware when the write finishes. If esptool can't connect, see manual download mode below.

## Manual download mode

Hold **BOOT (SW2)**, tap **RESET (SW1)**, release BOOT. The badge is now in the ROM serial bootloader waiting for esptool.

## Recovery

### Badge is stuck "waiting for download" after flashing

The S3's native USB wires the host's **DTR line toward GPIO0**, so serial monitors that assert DTR can hold the badge in download mode across resets. Fixes:

- Run any esptool command ending in a reset, e.g. `esptool --chip esp32s3 --port PORT flash-id` — it exits via "Hard resetting via RTS pin..." which boots the app.
- Close any open serial monitor before flashing or resetting (one client per port!).
- When scripting your own serial reader: hold DTR **steadily** asserted and never toggle it; toggling is what re-traps the chip.

### Badge boot-loops with `rst:0x7 (TG0WDT_SYS_RST)` and `ets_loader.c 78`

The flash image's bootloader header says QIO, which the ROM loader can't use on this flash chip (the second-stage bootloader is what switches to QIO). Reflash a known-good image, or if you're building merged images yourself: **`esptool merge-bin --flash-mode dio`**, never `qio`.

### Nuclear option

```bash
esptool --chip esp32s3 --port PORT erase-flash   # ~8 s, wipes everything incl. saved settings
```
Then flash a factory image.

## Reading the serial console

Firmware in this repo routes its console to the USB port. Any terminal at 115200 baud works, **but** prefer one that doesn't toggle DTR on open (see trap above). `pio device monitor` and most IDE monitors are fine while the app is running — just close them before flashing.

## Building from source

The examples are PlatformIO projects:

```bash
pip install platformio
cd examples/sd-test
pio run                                    # build
pio run -t upload --upload-port PORT       # flash
```

The board definition ([examples/sd-test/boards/defcon-badge.json](../examples/sd-test/boards/defcon-badge.json)) encodes the badge: ESP32-S3, 8 MB flash (QIO), 2 MB quad PSRAM.
