# Reticulum (RNode)

The badge as an [RNode](https://unsigned.io/rnode/): a host-controlled
[Reticulum](https://reticulum.network) LoRa transceiver. Plug it into a
computer or phone and use it with `rnsd`, NomadNet, MeshChat, or Sideband —
the badge handles the 915 MHz radio, the host runs the network stack.

Canonical repo (source, flash/provision scripts, Nibble Zero builds):
**[RetiaLLC/NibbleReticulum](https://github.com/RetiaLLC/NibbleReticulum)**

> ⚠️ **Attach a 915 MHz antenna (U.FL) before transmitting.** Transmitting
> without an antenna can damage the radio's PA. Receiving is safe.

## Flash it

```bash
pip install esptool rns
esptool --chip esp32s3 --port <PORT> write-flash 0x0 rnode-1.85-tft.factory.bin
```

Then provision the EEPROM (one time — makes it a valid homebrew RNode):

```bash
rnodeconf <PORT> -r --platform 80 --product f0 --model fe --hwrev 1
rnodeconf --info <PORT>   # should report: RNode v1.85, Normal (host-controlled)
```

Or let the automated script in NibbleReticulum do both:
`./DefconBadge/flash_dcbadge.sh <PORT>`.

| File | What |
|---|---|
| `rnode-1.85-tft.factory.bin` | **Recommended.** RNode v1.85 with the status screen on the badge's ILI9341 (mode, frequency, airtime, channel load, signal quality — 2.5x scale, full panel height). Flash at `0x0`. |
| `rnode-1.85.factory.bin` | Same firmware, headless (display dark). Flash at `0x0`. |

## Use it with Reticulum

Add to your `~/.reticulum/config` (LA-area community channel shown —
see the [NibbleReticulum SIGINT survey](https://github.com/RetiaLLC/NibbleReticulum/blob/main/SIGINT.md)):

```ini
[[RNode Badge]]
  type = RNodeInterface
  enabled = yes
  port = /dev/ttyACM0
  frequency = 914875000
  bandwidth = 125000
  spreadingfactor = 8
  codingrate = 5
  txpower = 17
```

Verified on hardware: bidirectional badge ↔ Nibble Zero Reticulum link,
3/3 packets delivered with proofs (914.875 MHz / SF8 / 125 kHz / CR5).

## Notes for builders

- Source: [`RetiaLLC/RNode_Firmware` branch `retia-stable`](https://github.com/RetiaLLC/RNode_Firmware/tree/retia-stable),
  targets `make firmware-retia_dcbadge_tft` (display) / `make firmware-retia_dcbadge` (headless, board ID `0x47`)
- `MODEM SX1276` only — the RFM95W's DIO1/BUSY are not routed
- The badge's SPI pins are **not** the ESP32-S3 defaults; the driver needs the
  explicit `SPI.begin(13, 12, 11, 48)` (already handled on the branch)
- Touch and SD chip-selects (14/39) are parked high at boot; the radio driver
  must never call `SPI.end()` on this board (shared display bus)
- Status: green debug LED (GPIO 2) blinks RX/TX; first NeoPixel = status color
