# hello-badge

Your first badge program: heartbeat on the green LED, a beep and a console line for every button press (each button gets its own pitch — B is the loudest, it sits on the piezo's 4 kHz resonance).

```bash
pio run -t upload --upload-port <PORT>
pio device monitor -p <PORT> -b 115200
```

Press buttons. That's it. Copy this project as the starting point for your own firmware — the board definition in `boards/` already knows the badge's flash/PSRAM setup.
