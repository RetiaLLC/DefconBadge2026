# MeshCore on the badge

Off-grid LoRa mesh messaging — [MeshCore](https://meshcore.io) v1.16.0 ported to the badge (SX1276 DIO0-only radio, ILI9341 + XPT2046 touch, all on the shared SPI bus). US 915 MHz preset by default; hardware-verified badge-to-badge with delivery ACKs.

**Flashable images live on the [`meshcore-v1.0.0` release](https://github.com/RetiaLLC/DefconBadge2026/releases/tag/meshcore-v1.0.0):**

| Image | What it is |
|---|---|
| `meshcore-touch-messenger.factory.bin` | **The one to use** — full standalone touchscreen mesh communicator (chats, contacts, channels, on-screen keyboard, RF monitor/spectrum apps, chimes, D-pad nav) that is *also* a BLE companion for the phone app |
| `meshcore-companion-ble.factory.bin` | Badge as a radio for the MeshCore phone app; pairing PIN shows on the badge screen |
| `meshcore-companion-usb.factory.bin` | Companion over USB serial for `meshcore-cli` / meshcore.js / the web client |

```bash
esptool --chip esp32s3 --port <PORT> write-flash 0x0 meshcore-touch-messenger.factory.bin
```

Source: [RetiaLLC/MeshCore](https://github.com/RetiaLLC/MeshCore/tree/retia-badge) (board variant) · [RetiaLLC/wadamesh](https://github.com/RetiaLLC/wadamesh/tree/retia-badge) (touch messenger). Repeater and room-server roles build from the same variant (`Retia_DCBadge_repeater` / `_room_server`).

**Badge-launcher note:** these are factory images with their own partition tables — they are **not** launcher SD-card guests. The touch messenger's app image (2.7 MB) exceeds the launcher's 2.375 MB shared slot, and MeshCore stores its identity/contacts in the same flash regions Meshtastic uses for settings, so running it as a guest would clobber them. Flash over USB instead (10 seconds), and hold UP at reset any time to get back to the launcher's factory image if you have it installed.
