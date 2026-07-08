# Hardware Guide — 2024 DEF CON Badge

A sheet-by-sheet walkthrough of the design ([schematic.pdf](../hardware/schematic.pdf), KiCad 8 sources in [hardware/kicad](../hardware/kicad/)).

## System overview

```
2×AA ──SW9──F1──► TPS61023 boost (5.1V) ─┐
                                          ├─► +5V rail ──► TLV62569 buck ──► +3.3V rail
USB-C VBUS ──────────► diode D10 ────────┘
```

- **ESP32-S3-WROOM-1** module (8 MB flash, 2 MB PSRAM)
- **MSP2402** 2.4″ display module: ILI9341 240×320 TFT + XPT2046 resistive touch + micro-SD slot
- **RFM95W** 915 MHz LoRa radio (SX1276) with U.FL antenna connector
- 10× **WS2812B-2020** NeoPixels (two "ears" of 5)
- Piezo buzzer, 6 face buttons + BOOT/RESET, dedicated micro-SD slot
- SAO v2, QWIIC, UART, SPI accessory headers, off-board NeoPixel connector

## Power (sheet 7)

Two power sources, diode-OR'd onto the +5 V rail:

- **Battery path:** 2×AA → power switch SW9 → polyfuse F1 → **TPS61023** boost converter set to **5.1 V** (R27 750k / R28 100k). The extra 0.1 V biases D10's drop so USB wins when both are present.
- **USB path:** VBUS → diode D10 → +5 V rail. A MOSFET (Q4) **disables the boost converter whenever USB is present** — so battery current is zero on USB power, and firmware can treat "USB plugged in" as "not on battery".
- **3.3 V:** TLV62569 buck (R30 453k / R31 100k → 3.318 V) feeds the MCU, radio, display logic.
- Reverse-battery indicator: red/green LED pair across the input.

Consequences for firmware: no brownout mystery — a badge that resets under load on AA power is hitting the boost converter's current limit (NeoPixels + radio TX + display are a real load). The display backlight is a constant ~0.2–0.35 W drain you cannot switch off.

## MCU block (sheet 2)

- SW1 = RESET (EN), SW2 = BOOT (GPIO0) — the standard ESP32 programming pair.
- Green debug LED on GPIO2 (also on the SAO header).
- 32.768 kHz crystal on GPIO15/16 for RTC deep-sleep timing.
- Full pin map: see [pinout.md](pinout.md).

## Display + touch + module SD (sheet 4)

The MSP2402 module carries three devices; the badge fans all of them onto the shared SPI bus through DR-series resistor networks:

- **TFT (ILI9341):** CS 47, DC 40, RST 41 — wired correctly, runs happily at 40 MHz. Backlight pin is tied to 3.3 V (always on).
- **Touch (XPT2046):** CS 14, IRQ 42, clock shared — but **T_DIN/T_DO are swapped** relative to the bus (T_DO→MOSI net, T_DIN→MISO net). Hardware SPI cannot reach it; reversed-role bit-banging can. IRQ works and is the cheap "is anyone touching" signal.
- **Module SD slot:** CS 39 — **same MISO/MOSI swap**, so leave it alone and use the dedicated slot below.

## Dedicated micro-SD (sheet 5)

Correctly wired SPI slot: CS **10** (10 kΩ pull-up), plus the shared bus pins. Verified working at 20 MHz with a 16 GB SDHC card — see [sd-card.md](sd-card.md) and [examples/sd-test](../examples/sd-test/).

## Buttons (sheet 8)

Six face buttons — LEFT 3, UP 4, DOWN 5, RIGHT 6, B 7, A 8 — all to ground with 10 kΩ pull-ups. Footprints for RC debounce caps (C7–C12) exist but are **not populated**; debounce in software (~40 ms works well).

## Buzzer (sheet 3)

GPIO9 → 330 Ω → NMOS gate (10 kΩ pull-down) → **PS1240P02CT3 piezo** from +5 V with 1 kΩ parallel damping resistor. It's a resonant tweeter, loudest around 4 kHz:

- Simple beeps: LEDC PWM square wave, 2–5 kHz. Loud and clean.
- Sampled audio: 1-bit sigma-delta (or LEDC-PWM DAC) works, but off-resonance output is quiet and gritty. Apply heavy gain + clipping. (The Doom build does exactly this.)

## LoRa radio (sheet 9)

**RFM95W-915S2** (SX1276): CS 48, RESET 38, DIO0/IRQ 21, shared SPI. DIO1+ are **not connected** — SX127x-style drivers only. Antenna via U.FL (J1) or the TP1 wire pad.

> ⚠️ **Never transmit without a 915 MHz antenna attached** — you can damage the PA. Receiving is safe.

## NeoPixels (sheets 10, 6)

- On-board: GPIO17 → 10× WS2812B-2020, right ear first (D2,D6,D8,D5,D7), then left (D9,D11,D12,D13,D14). Powered from 3.3 V (works fine for WS2812B at 3.3 V logic).
- Off-board connector J8: **GPIO0** through a proper MOSFET level shifter (Q2) to 5 V logic, with 5 V supply on the header. Note the GPIO0 double-duty: driving external pixels means the pin also feels button SW2 and the boot strap — keep it high-Z at reset.

## Connectors (sheet 6)

| Ref | What | Pins |
|---|---|---|
| J3 | SPI accessory (unpopulated) | MISO 12, MOSI 11, SCK 13, CS 37, 3.3 V, GND |
| J4 | UART | TX 43, RX 44 (330 Ω series), 3.3 V, GND |
| J5 | **SAO v2** | SDA 35, SCL 36, GPIO1, GPIO2, 3.3 V, GND |
| J6 | **QWIIC** | SDA 35, SCL 36, 3.3 V, GND |
| J7 | USB-C | D− 19, D+ 20, CC pulldowns populated |
| J8 | NeoPixel out | GPIO0 (5 V level-shifted), 5 V, GND |
| J9 | 2×AA battery | via SW9 + polyfuse |

I²C on GPIO35/36 was picked deliberately for software compatibility with existing HakCat Nugget projects.
