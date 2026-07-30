# ESP32 Bus Pirate

A port of the open-source **[ESP32 Bus Pirate](https://github.com/KonradIT/ESP32-Bus-Pirate)** (geo-tp / KonradIT) to the badge — a multi-protocol hardware-hacking multitool driven from the badge TFT, USB-serial CLI, or a Wi-Fi web CLI.

Source & release: **[RetiaLLC/ESP32-Bus-Pirate @ badge-v1.0](https://github.com/RetiaLLC/ESP32-Bus-Pirate/releases/tag/badge-v1.0)** (branch `retia-badge-port`, all badge support gated behind `-DDEVICE_RETIA_BADGE`).

## ⚠ Read before flashing — transmit- & attack-capable
Alongside the benign bus tools this firmware can **transmit and attack**: Wi-Fi (scan/sniff/**deauth**/nmap), Bluetooth LE (scan/**spoof**/sniff), LoRa/Sub-GHz **transmit**, and bus/RF **jam**. It ships as a full-capability image — not a passive build.

**Use only on hardware, networks, and RF you own or are authorized to test.** Transmitting against systems you don't own may be illegal. You are responsible for lawful use.

## What works on the badge (verified on hardware)
- **ILI9341 TFT UI** + **d-pad/A·B** nav; auto-boots to the USB-serial CLI.
- **I²C** — bus scan and live register **monitor mirrored to the TFT**; `identify`/`read`/`dump`.
- **`bme`** — decodes a **BME280** (temp/humidity/pressure) on screen and colours the ear NeoPixels by temperature.
- **LED** — drives the 10 ear NeoPixels (fill / rainbow / chase / wave).
- **Wi-Fi** and **BLE** scanners.
- **LoRa** (on-board RFM95W / SX1276) — send / receive / sniff / RSSI, **TX + RX verified badge-to-badge** (new SX127x driver).

## Wiring targets
I²C on the **SAO header (J5)**: 3V3 / GND / SDA=GPIO35 / SCL=GPIO36. UART on J4 (TX 43 / RX 44). ⚠ The badge's Qwiic connectors are pin-reversed vs. the standard — use **J5**, not Qwiic.

## Flash it
Browser (recommended, with the authorization prompt): **[catbadge.online](https://catbadge.online)** or **[scriptkitty.sh](https://scriptkitty.sh)**. Or with esptool:
```bash
esptool --chip esp32s3 --before default-reset --after hard-reset \
  write-flash --flash-mode dio --flash-freq 40m --flash-size 8MB 0x0 buspirate-badge.factory.bin
```

`buspirate-badge.factory.bin` — SHA-256 `9d86bb7b9229dd5b36ede335f711a9949c9cc4ccefdd1b8f25daaa4c6bff75d2`
