# ESP32 Bus Pirate on the DEF CON Badge — Beginner's Guide

> Turn your badge into a **multi-protocol hardware & radio analysis multitool** with
> a live color display. This guide gets you from "just flashed" to running eye-catching,
> hands-on demos — **no laptop screen required, the badge shows you everything.**
>
> Everything here is tested on the actual badge on a bench. Commands and on-screen
> results are what the badge really does.

---

## 1. What you've got

The Bus Pirate speaks a pile of protocols — I²C, SPI, UART, 1-Wire, LoRa, Wi-Fi,
Bluetooth LE and more — and now **mirrors the interesting ones to the badge's TFT**.
You drive it three ways, pick whichever is handy:

- **USB serial** — plug the badge into a computer, open a serial terminal at **115200 baud**. The badge auto-boots straight into this CLI.
- **Wi-Fi web CLI** — the badge hosts an access point; connect and use the CLI in your browser.
- **Browser Web Serial** — flash + drive it entirely from [catbadge.online](https://catbadge.online) with no software installed.

Every command below is typed at the CLI prompt.

> ⚠️ **Radio safety & authorization.** This build can transmit (LoRa/Wi-Fi/BLE) and
> includes attack-capable commands (deauth, spoof, jam, spam). **Only test hardware,
> networks and RF you own or have written permission to test.** This guide sticks to
> **passive, receive-only demos** — the safe, always-legal stuff. Never transmit on
> LoRa without a 915 MHz antenna attached, or you can damage the radio.

---

## 2. First contact

1. **Flash** the badge (browser flasher at [catbadge.online](https://catbadge.online), or `esptool … write-flash 0x0 buspirate-badge.factory.bin`).
2. **Tap RESET.** It boots to the USB-serial CLI at **115200**.
3. Type `help` for the command list, or `mode` to see the protocols. Type `mode <name>` to enter one (e.g. `mode i2c`). The prompt shows the current mode (`I2C>`, `LORA>`, …).

> 💡 **Entering a mode asks a few setup questions** (pins, speed, radio settings).
> Each shows its default in brackets like `SDA GPIO [35]:` — **just press ENTER to
> accept every default.** The examples below assume you took the defaults. To bail
> out of any running command, press **ENTER**; `mode hiz` returns to a safe idle.

That's it. Now let's make the screen do things.

---

## 3. LoRa — the visual showpieces ⭐

LoRa is where the on-screen demos shine. Enter the mode:

```
mode lora
```

It reports the on-board radio (`RFM95W / SX127x`) and asks *"Configure radio settings?"* —
answer **N** to take the sensible defaults. You're now at `LORA>`.

> Attach the **915 MHz antenna** before any command that transmits (`send`, `spam`).
> The receive/scan demos below are safe without one, but signals will be weak.

### 3a. Waterfall — a spectrum analyzer on your badge
```
waterfall
```
Answer the prompts (defaults are fine — try Start `902`, End `928`, Step `0.1`,
Hold `40`, Threshold `-90`). The screen paints a **live heat-map spectrum**: cool
blue/green where the band is quiet, hot **yellow/red where something's transmitting**.
The header shows the peak frequency. Press **ENTER** to stop.

*Try it:* narrow the range to Start `914` End `916` and have a second badge `send`
on 915 — watch a hot bar burn up the center.

### 3b. RSSI meter — "find the transmitter"
```
rssi
```
A big live **signal-strength readout** with a colored bar and a held-peak marker.
Move a transmitter closer and the bar climbs and turns red. Great booth interaction.
Press **ENTER** to stop.

### 3c. Band scanner — who's on the air
```
scan
```
Sweeps a frequency range and paints a **peak-hold heat spectrum** — every channel
that's been active lights up and stays lit, so you can see band occupancy at a glance.
Prints the strongest hits to serial too. ENTER to stop (it offers to save the best freq).

### 3d. Packet sniffer — watch packets land
```
receive
```
Parks the radio in RX and shows a **rolling list of received frames on screen** —
newest on top, each with its number, **RSSI**, length and an ASCII preview of the
payload. Have another badge `send hello world` and watch it appear. ENTER to stop.

### 3e. Meshtastic monitor — decode real mesh traffic
The badge speaks **real Meshtastic**. Enter the mesh sub-shell:
```
mesh
```
Then from the numbered menu: **2** = pick preset `LongFast`, **3** = set frequency
`906.875` (US default), **4** = set channel key `AQ==` (the public key), **7** =
**Receive packets**. The screen becomes a **Mesh Monitor** showing decoded packets —
`!nodeid` + the text message or packet type — for any Meshtastic traffic on the
default channel. (Verified decoding live packets from a stock Meshtastic node.)

To transmit a mesh message from the badge, use menu option **6** (Send text) — a real
Meshtastic node or phone app will receive it.

### 3f. MeshCore sniffer
```
meshcore
```
One command tunes the radio to **MeshCore's** default US PHY (915 MHz / SF10) and
drops into the on-screen sniffer, so MeshCore frames show up as RSSI + raw bytes.
(MeshCore group payloads are AES-128 encrypted, so they show raw, not decoded — and
because MeshCore uses a different frequency/spreading-factor than Meshtastic, it's a
separate listening mode, not simultaneous.)

### 3g. Send between two badges
On badge A: `receive`. On badge B: `setfreq 915` then `send hello from the other badge`.
Watch it pop up on A's screen with RSSI/SNR.

---

## 4. I²C — live sensor dashboard

Best hands-on demo if you have a sensor. **Wire the sensor to the SAO header (J5)** —
**not** the Qwiic/STEMMA connectors (they're pin-reversed on this board rev, see
`docs/hardware-errata.md`). J5 pinout: **1 = 3V3, 2 = GND, 3 = SDA (GPIO35), 4 = SCL (GPIO36).**

```
mode i2c
```
It asks `SDA GPIO [35]:`, `SCL GPIO [36]:`, `Frequency [100000]:` — **press ENTER
three times** to take the badge's SAO defaults. Then at `I2C>`:
```
scan
```
The screen shows an **on-screen device list**, and serial prints `Found device at
0x76`. A BME280/BMP280 environment sensor answers at `0x76` (or `0x77`).

```
bme 0x76
```
Reads and **decodes the sensor onto the badge**: a two-column **BME280** dashboard
showing **T / H / P** (e.g. `T 24.9 C · H 49 %RH · P 1001 hPa`), with the raw
register values alongside — and, the fun part, **the ear LEDs change color with the
temperature.** Warm the sensor with your fingers and watch the reading climb and the
ears shift. Press ENTER to stop.

Also handy: `monitor <addr>` mirrors live register reads to the screen.

---

## 5. Wi-Fi — scan the air

```
mode wifi
scan
```
Lists nearby **access points** — SSID, channel, signal, security. Fully passive.

There's also a `waterfall` here (2.4 GHz channel activity) and a `sniff` for beacon/
probe frames. (The `deauth`, `spam`, `spoof` commands are transmit/attack — authorized
use only, not part of this guide.)

---

## 6. Bluetooth LE — scan for devices

```
mode bluetooth
scan
```
Lists nearby **BLE advertisers** — MAC, RSSI, name/flags. You'll see a sea of phones
and earbuds broadcasting proximity beacons (Apple Continuity, Google Nearby), all with
**randomized MACs** — a nice live lesson in BLE privacy.

---

## 7. UART — sniff a live serial line

UART shines when you point it at a **real serial source** — e.g. the bench's own UART
console. Wire the badge's UART **RX (GPIO44)** to the source's **TX**, and **GND to GND**.

```
mode uart
autobaud
```
`autobaud` auto-detects the line's baud rate. Then:
```
snifftxt
```
streams the decoded text traffic as it flows by (`sniffraw` for raw hex). This turns
the badge into a passive serial tap — great for reading what a device is chattering.

---

## 8. LED ears — because it's a badge

```
mode led
```
Answer the setup (LED count **10**, low brightness like **40** — the ears are bright
and high brightness on USB power can brown out the board). Then:
```
rainbow
```
for a moving rainbow, or `fill 00FF00` for solid green, `fill 000000` to turn them off.

---

## 9. What needs external gear

Some Bus Pirate modes need a **target device** to be interesting and don't demo well
bare on a bench:

- **SPI** (`mode spi`) — needs an SPI flash/EEPROM/sensor wired to the SAO/accessory
  jumpers to `sniff` or dump. Great for chip-dumping, not a standalone demo.
- **1-Wire, JTAG, and the exotic buses** — same story: bring a target.

The badge's usable-out-of-the-box modes are **I²C, UART, 1-Wire, LED (ears), USB-HID,
BLE, Wi-Fi, and LoRa**; SPI/2-wire/3-wire/JTAG need jumpers to the SAO header.

---

## 10. Quick reference

| Mode | Enter | Star demo | Also |
|---|---|---|---|
| **LoRa** | `mode lora` | `waterfall`, `rssi`, `scan`, `receive`, `mesh`→7 | `send`, `meshcore`, `status` |
| **I²C** | `mode i2c` | `scan`, `bme 0x76` | `monitor`, `ping`, `read` |
| **Wi-Fi** | `mode wifi` | `scan` | `waterfall`, `sniff`, `status` |
| **BLE** | `mode bluetooth` | `scan` | `sniff`, `status` |
| **UART** | `mode uart` | `autobaud` → `snifftxt` | `sniffraw`, `scan` |
| **LED** | `mode led` | `rainbow` | `fill RRGGBB` |

**Anywhere:** `help` (commands for the current mode), `mode` (list modes), `mode hiz`
(safe idle), ENTER (stop a running command).

---

*Ported from [KonradIT/ESP32-Bus-Pirate](https://github.com/KonradIT/ESP32-Bus-Pirate).
On-screen LoRa/I²C demos and the Meshtastic/MeshCore tooling are Retia badge additions.
Transmit- & attack-capable — authorized use only.*
