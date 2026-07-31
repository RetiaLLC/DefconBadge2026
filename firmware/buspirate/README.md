# ESP32 Bus Pirate

A port of the open-source **[ESP32 Bus Pirate](https://github.com/KonradIT/ESP32-Bus-Pirate)** (geo-tp / KonradIT) to the badge — a multi-protocol hardware-hacking multitool driven from the badge TFT, USB-serial CLI, or a Wi-Fi web CLI.

Source & release: **[RetiaLLC/ESP32-Bus-Pirate @ badge-v1.2](https://github.com/RetiaLLC/ESP32-Bus-Pirate/releases/tag/badge-v1.2)** (branch `retia-badge-port`, all badge support gated behind `-DDEVICE_RETIA_BADGE`).

**New to it? Read the [beginner's guide](GUIDE.md)** — walks through every on-screen demo you can run on a bench.

## ⚠ Read before flashing — transmit- & attack-capable
Alongside the benign bus tools this firmware can **transmit and attack**: Wi-Fi (scan/sniff/**deauth**/nmap), Bluetooth LE (scan/**spoof**/sniff), LoRa/Sub-GHz **transmit**, and bus/RF **jam**. It ships as a full-capability image — not a passive build.

**Use only on hardware, networks, and RF you own or are authorized to test.** Transmitting against systems you don't own may be illegal. You are responsible for lawful use.

## What works on the badge (verified on hardware)
- **ILI9341 TFT UI** + **d-pad/A·B** nav; auto-boots to the USB-serial CLI.
- **LoRa on-screen suite** (on-board RFM95W / SX1276, new SX127x driver):
  - **`waterfall`** — heat-map spectrum analyzer (blue→red), peak-hold.
  - **`receive`** — packet sniffer: a rolling on-screen list (#, RSSI, length, ASCII).
  - **`rssi`** — big live signal meter + held-peak marker.
  - **`scan`** — peak-hold heat spectrum across a band.
  - **`mesh` → Receive** — Meshtastic monitor, decoded packets on-screen; verified vs a **real Meshtastic node**. `send` for TX.
  - **`meshcore`** — tunes to MeshCore's 915/SF10 PHY and sniffs (payloads AES-128, shown raw).
  - Core `send`/`receive`/`sniff`/`setfreq` **TX + RX verified badge-to-badge**.
- **I²C** — bus scan + live register **monitor on the TFT**; **`bme`** decodes a **BME280** (temp/humidity/pressure) on screen and colours the ear NeoPixels by temperature.
- **LED** — 10 ear NeoPixels (fill / rainbow / chase / wave).
- **Wi-Fi** and **BLE** scanners; **UART** sniff/autobaud.

## Wiring targets
I²C on the **SAO header (J5)**: 3V3 / GND / SDA=GPIO35 / SCL=GPIO36. UART on J4 (TX 43 / RX 44). ⚠ The badge's Qwiic connectors are pin-reversed vs. the standard — use **J5**, not Qwiic.

## Flash it
Browser (recommended, with the authorization prompt): **[catbadge.online](https://catbadge.online)** or **[scriptkitty.sh](https://scriptkitty.sh)**. Or with esptool:
```bash
esptool --chip esp32s3 --before default-reset --after hard-reset \
  write-flash --flash-mode dio --flash-freq 40m --flash-size 8MB 0x0 buspirate-badge.factory.bin
```

`buspirate-badge.factory.bin` — SHA-256 `4c87990b5b56fe5b4007f013dbbde34513b6656ed42f592d1bef9c74482c860f`

## Known limitation — Wi-Fi web CLI terminal
The badge **serves** the web CLI page over Wi-Fi (hotspot or client), but the interactive
terminal uses a **single-client WebSocket** that is unreliable in real browsers (especially
phones): output can fail to return and the page reconnect-loops with *"Connection lost."*
This is an upstream `/ws` issue; two fixes were attempted (socket headroom; async send via
`httpd_queue_work`) without fully resolving it. **Use USB serial or browser Web Serial
([catbadge.online](https://catbadge.online)) for reliable interactive use.**
