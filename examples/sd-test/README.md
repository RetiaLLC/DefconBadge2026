# sd-test

Smoke test for the badge's micro-SD slot (the dedicated one, CS GPIO10 — not the crossed-wire slot on the display module; see [docs/sd-card.md](../../docs/sd-card.md)).

Mounts the card FAT (**formatting it if it doesn't mount** — erases the card!), prints card info, writes `/sd/badge_test.txt`, reads it back.

```bash
pio run -t upload --upload-port <PORT>
pio device monitor -p <PORT> -b 115200
```

- Green LED blinking **slow** = PASS
- Green LED blinking **fast** = FAIL (is a card inserted? FAT-formattable?)

Verified on hardware with a 16 GB SDHC card at 20 MHz.
