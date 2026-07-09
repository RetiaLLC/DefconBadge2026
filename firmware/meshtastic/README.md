# Meshtastic

The badge is a fully working [Meshtastic](https://meshtastic.org) node: 915 MHz LoRa mesh, TFT UI, buttons, BLE to the phone app. Firmware 2.7.23 with a badge-specific variant (`hwModel PRIVATE_HW`).

> ⚠️ **Attach a 915 MHz antenna (U.FL) before joining a mesh.** Transmitting without an antenna can damage the radio's PA. And set your region before use:
> ```bash
> pip install meshtastic
> meshtastic --port <PORT> --set lora.region US
> ```

## Flash it

```bash
esptool --chip esp32s3 --port <PORT> write-flash 0x0 meshtastic-2.7.23-standard.factory.bin
```

| File | What |
|---|---|
| `meshtastic-2.7.23-standard.factory.bin` | Stock-behavior build for the badge variant (classic button-driven TFT UI). |
| `meshtastic-2.7.23-lowpower.factory.bin` | Power-tuned build: idle draw cut ~39% (NeoPixels off by default, BLE off until enabled by phone), B button = Back/Cancel, 40 ms debounce, battery light-sleep available as opt-in (`--set power.is_power_saving true` — battery-only deployments; USB presence inhibits sleep). |
| `meshtastic-2.7.23-mui-touch.factory.bin` | **Experimental color touch UI** (Meshtastic device-ui / LVGL, `touchscreen-build-bleoff`). Full touchscreen operation out of the box — on-screen keyboard, tap navigation — despite the badge's crossed touch data lines (worked around with a calibrated bit-banged XPT2046 driver). D-pad + A/B also work as a keypad. Bluetooth is off in this build (it conflicts with the touch UI's home screen); phone/client connectivity is WiFi/TCP (port 4403). Showcase build; the low-power build is the daily driver. |

A Meshtastic badge enumerates over USB with its MAC in the port name (e.g. `usbmodem744DBD216494`) — that's normal.

## Variant files

`variant.h` / `pins_arduino.h` in this directory are the badge variant used to build these images (RFM95W on the shared SPI bus, ILI9341 TFT, buttons as trackball input, NeoPixel ambient lighting). Drop them into `variants/esp32s3/` in the Meshtastic firmware tree to build your own.

Notes for builders:
- `USE_RF95` only — the radio's DIO1/BUSY are not routed, SX126x variants won't work
- USB serial console carries the protobuf client API, **not** a text log (human-readable log is on UART J4)
- The badge has no battery-voltage sense; don't enable battery-percentage features
