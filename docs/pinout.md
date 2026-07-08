# GPIO Pinout — 2024 DEF CON Badge (ESP32-S3)

MCU: **ESP32-S3-WROOM-1** module — dual-core LX7 @ 240 MHz, **8 MB flash** (GigaDevice, QIO-capable), **2 MB embedded PSRAM** (quad, 80 MHz), native USB.

> ⚠️ Read the [quirks](#hardware-quirks) section before writing firmware. Several pins are shared, one pair is crossed in hardware, and the native-USB download mode has a trap.

## Complete GPIO map

| GPIO | Function | Direction | Notes |
|-----:|----------|-----------|-------|
| 0 | **BOOT button (SW2**, silkscreen "GPIO_0"**)** + off-board NeoPixel connector J8 (level-shifted to 5 V) | in / out | Strap pin: hold low through a reset → ROM download mode. Free as a button input after boot. |
| 1 | SAO connector GPIO1 (J5 pin 5) | any | |
| 2 | **Green debug LED** (330 Ω) + SAO GPIO2 (J5 pin 6) | out | |
| 3 | **LEFT button (SW3)** | in | Active-low, external 10 kΩ pull-up. Debounce caps C7–C12 are DNP (not populated). |
| 4 | **UP button (SW4)** | in | Active-low, 10 kΩ pull-up |
| 5 | **DOWN button (SW5)** | in | Active-low, 10 kΩ pull-up |
| 6 | **RIGHT button (SW6)** | in | Active-low, 10 kΩ pull-up |
| 7 | **B button (SW7)** | in | Active-low, 10 kΩ pull-up |
| 8 | **A button (SW8)** | in | Active-low, 10 kΩ pull-up |
| 9 | **Buzzer** (silkscreen "MIDI SPEAKER"; 330 Ω → NMOS gate → PS1240P02CT3 piezo from +5 V, 1 kΩ parallel) | out | Square wave ≈4 kHz is loudest (piezo resonance). 1-bit sigma-delta works for sampled audio (quiet, crunchy). |
| 10 | **micro-SD card CS** (dedicated slot, 10 kΩ pull-up) | out | Shares the SPI bus. Verified working — see [sd-card.md](sd-card.md). |
| 11 | **SPI MOSI** (shared bus) | out | Also: touch T_DO and module-SD "SD_MISO" land here — see quirks. |
| 12 | **SPI MISO** (shared bus) | in | Also: touch T_DIN and module-SD "SD_MOSI" land here — see quirks. |
| 13 | **SPI SCK** (shared bus) | out | Display runs at 40 MHz; SD at 20 MHz; touch ≤2.5 MHz. |
| 14 | **Touch controller CS** (XPT2046 on display module) | out | |
| 15 | 32.768 kHz crystal (XTAL_32K_P) | — | Not available as GPIO |
| 16 | 32.768 kHz crystal (XTAL_32K_N) | — | Not available as GPIO |
| 17 | **On-board NeoPixels** — 10× WS2812B-2020, right ear (5) then left ear (5) | out | |
| 18 | Broken out to MCU_gpio bus, unused on the board | any | |
| 19 | USB D− | — | |
| 20 | USB D+ | — | |
| 21 | **LoRa DIO0 / IRQ** (RFM95W) | in | |
| 35 | **I²C SDA** — SAO (J5) and QWIIC (J6) | any | Chosen for software compatibility with HakCat Nuggets (no dedicated I²C pins). |
| 36 | **I²C SCL** — SAO (J5) and QWIIC (J6) | any | |
| 37 | **Accessory SPI CS** (J3, unpopulated header) | out | |
| 38 | **LoRa RESET** (RFM95W, 10 kΩ pull-up) | out | |
| 39 | **Display-module SD slot CS** | out | ⚠️ That slot's data lines are crossed — see quirks. |
| 40 | **TFT DC** (data/command) | out | |
| 41 | **TFT RESET** | out | |
| 42 | **Touch IRQ** (XPT2046 PENIRQ, active low) | in | |
| 43 | **UART TX** (J4 accessory header, 330 Ω series) | out | Default ESP-IDF console is here unless retargeted to USB. |
| 44 | **UART RX** (J4) | in | |
| 45 | Unconnected | — | Strap pin (VDD_SPI voltage) — leave alone |
| 46 | Unconnected | — | Strap pin — leave alone |
| 47 | **TFT CS** (ILI9341) | out | |
| 48 | **LoRa CS** (RFM95W, 10 kΩ pull-up) | out | |

## The shared SPI bus

One SPI bus (SCK 13 / MOSI 11 / MISO 12) serves **five** peripherals, each with its own chip-select:

| Device | CS | Max clock |
|---|---|---|
| ILI9341 TFT (240×320) | 47 | 40 MHz (overclocked, works) |
| RFM95W LoRa | 48 | 10 MHz |
| micro-SD slot (dedicated) | 10 | 20 MHz |
| SD slot on display module | 39 | — (crossed lines, unusable as-is) |
| XPT2046 touch | 14 | 2.5 MHz (crossed lines — bit-bang only) |
| Accessory header J3 | 37 | — |

**Rule: before using any bus device, drive every other CS high.** A floating CS (especially the LoRa radio's) will corrupt bus traffic.

## Hardware quirks

1. **Touch data lines are crossed.** The XPT2046's T_DIN (its input) is wired to GPIO12 — the bus **MISO** net — and T_DO (its output) to GPIO11, the bus **MOSI** net. Hardware SPI can therefore never talk to the touch controller. It *is* reachable by bit-banging with reversed pin roles (drive commands on 12, read data on 11) while holding the bus. T_IRQ (GPIO42, active low on touch) works normally.
2. **The display module's own SD slot has the same crossing** ("SD_MISO"→GPIO11/MOSI, "SD_MOSI"→GPIO12/MISO). Use the **dedicated** micro-SD slot (CS 10) instead — it's wired correctly and verified working.
3. **Native-USB download-mode trap.** The USB-Serial/JTAG peripheral wires host DTR to GPIO0. A serial monitor that asserts DTR, combined with a reset, drops the badge into ROM download mode ("waiting for download") instead of booting. Flash with esptool `--after hard-reset`, avoid DTR-toggling monitors, and see [flashing.md](flashing.md#recovery) for recovery.
4. **Display backlight is hardwired to 3.3 V** — always on, not software-controllable (~0.2–0.35 W). A white screen just means the panel is in sleep with the backlight lit.
5. **No battery voltage sense.** GPIO45/46 are unconnected; +BATT and +USB_IN are power-only. Firmware cannot read battery level — USB presence is the only "external power" signal (USB plugged in ⇒ battery path disabled).
6. **LoRa DIO1/BUSY are not routed.** Use SX127x/RF95 drivers only (`USE_RF95`, `LORA_DIO1 = NC`); SX126x variants will not work.
7. **8 MB flash despite what USB says.** Chip identification may report "FH4R2 (4 MB)" but the module carries an 8 MB GigaDevice part (`flash-id`: c8:4017). Partition freely up to 8 MB.
8. **Merged-image builders: use DIO flash mode.** A factory image with a QIO bootloader header boot-loops at ROM stage (`TG0WDT` reset + `ets_loader.c 78`). The second-stage bootloader enables QIO itself; the header must stay DIO.
