# Hardware Errata — 2024 DEF CON Badge

Known hardware bugs on the shipped/sample boards, with evidence and workarounds.

---

## ERRATA-01 — Qwiic / STEMMA-QT connectors are pin-reversed (power/ground swapped)

**Severity:** High — can damage attached sensors and prevent a host from booting.
**Affected refs:** **J6** (I²C Qwiic), **J4** (UART on a Qwiic-shaped connector).
**Also affects:** other Retia boards that use the same `retia_component_library` Qwiic
footprint (e.g. the Bluetooth Nugget) — the bug reproduces there identically.

### Symptom

Plug any standard SparkFun-Qwiic / Adafruit-STEMMA-QT sensor into J6 with a standard
QT cable and:

- the sensor's **power LED never lights** (it isn't getting valid 3.3 V),
- an I²C scan finds **nothing**,
- on a smaller host (Nugget) the board **won't even boot** with the sensor attached
  (the mis-connected sensor loads/browns out the 3.3 V rail),
- reproduced across **3 different Adafruit Qwiic sensors** → not a dead sensor.

### Root cause

The connector's **physical pin order is backwards relative to the Qwiic/STEMMA-QT
standard**, so a standard cable delivers **power and ground on the wrong pins**
(reversed/scrambled). Evidence from `hardware/kicad/`:

- Footprint: `retia_component_library:QWIIC_CONNECTOR_GENERIC_1x04-1MP_P1.00mm_Horizontal`
  — a **generic** `kicad-footprint-generator` JST-SH (SM04B-SRSS-TB) part, **not**
  SparkFun's official Qwiic footprint.
- Both J6 and J4 are placed on the **back copper (`layer "B.Cu"`)** of the board.
- The nets on the pads are individually correct — J6: pad1 GND, pad2 +3.3 V, pad3
  SDA/GPIO35, pad4 SCL/GPIO36 — **but** the combination of the generic footprint's
  pin-1 orientation and the back-layer (mirrored) placement means the pad that the
  cable's **GND** wire seats into is not the GND pad, and likewise for +3.3 V. The
  standard cable therefore reverse-/cross-powers the sensor.

The Qwiic/STEMMA-QT standard is a 4-pin JST-SH 1.0 mm connector, pin order
**GND · 3.3 V · SDA · SCL** (black/red/blue/yellow). The badge connector does not
present that order at the physical cable interface.

> Note: **J4 is a Qwiic-*shaped* UART port** (pads 3/4 are UART TX GPIO43 / RX GPIO44,
> silk "UART Connector"), physically identical to the I²C Qwiic J6 (silk "QWIIC
> CONNECTOR"). Even once the polarity is fixed, do not confuse the two.

### Confirm on a unit (30 s, no risk)

Power the board, **nothing plugged in**, meter the 4 contacts of J6: find which pin
reads **+3.3 V** and which reads **0 V/GND**. Compare to the QT cable (black = GND,
red = 3.3 V). If the badge's 3.3 V pin lines up with the cable's **black** wire (or
GND with red), the connector is reversed → do not plug stock sensors in as-is.

### Workaround (use today)

Wire I²C to the **SAO header J5** (0.1″ 2×3, unambiguous, meterable) — same I²C bus:

| J5 pin | Net | QT/sensor wire |
|---|---|---|
| 1 | +3.3 V | red / VIN / 3Vo |
| 2 | GND | black |
| 3 | SDA (GPIO35) | blue |
| 4 | SCL (GPIO36) | yellow |
| 5 / 6 | GPIO1 / GPIO2 | — |

**Meter J5 pins 1 & 2 for 3.3 V / GND before connecting.** Verified working: a
BME280 hand-wired to J5 enumerated at 0x76 and returned live data (chip-ID reg
0xD0 = 0x60; compensated temperature ≈ 26 °C).

Alternatively, a **polarity-reversed / hand-rewired QT cable** makes J6 usable, but
per-cable rework is error-prone — SAO is safer.

### Fix (next board rev)

In `retia_component_library`, correct the Qwiic connector so a standard STEMMA-QT/Qwiic
cable seats **GND→GND, 3.3 V→3.3 V**. Options:
- Replace the generic `kicad-footprint-generator` SM04B footprint with SparkFun's
  official Qwiic footprint (correct pin-1 keying), **or**
- Keep the part but fix the pad-to-pin-1 assignment / connector orientation so the
  physical cable interface matches the standard, **and** re-verify after the front/back
  layer mirror.
- Re-check every board that instantiates this shared footprint (badge J4/J6, Nugget,
  any others).

Verify the fix by metering the assembled connector against a known-good Qwiic host
before shipping.
