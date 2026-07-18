# PINGPONG Relationship Mirror

Local read-only diagnostics dashboard for the installation.

## Open the dashboard

From the repository root:

```bash
python3 -m http.server 8080 --directory webui
```

Then open:

```text
http://localhost:8080
```

Use desktop Chrome or Edge. Web Serial does not currently work in Firefox or
Safari. The dashboard must be served from `localhost`; opening `index.html`
directly does not give the browser permission to use Web Serial.

## Explore without hardware

Choose **Explore Demo**. This simulates:

- both moving rackets;
- radio packet health;
- racket and table contacts;
- a rally and connection levels;
- motor, light, and sound commands.

Demo game values are clearly labelled simulated.

## Connect TeensyMaster

1. Flash the updated TeensyMaster firmware.
2. Close Arduino Serial Monitor or any other program using its USB serial port.
3. Connect TeensyMaster over USB.
4. Choose **Connect Master**.
5. Select the Teensy serial device in the browser dialog.

The configured `115200` baud value is a Web Serial requirement; Teensy USB
serial itself is native USB and does not depend on that physical baud rate.

## Current truth versus future fields

Live now:

- raw gyroscope and accelerometer values from both rackets;
- orientation, swing speed, grip pressure, piezo input and hit strength;
- persistent racket and table hit counters;
- radio packet totals, packet rate, freshness, and packet age;
- motor command and right-racket AC-light trigger;
- telemetry connection and frame health.

Reserved for the next architecture stages:

- legal serve/rally phase and expected next contact;
- authoritative rally count;
- relationship level and level progress;
- exact LED, motor, light, and PlugData world state.

Until `GameEngine` is connected, the live dashboard labels those fields
**PENDING** and keeps them at Level 0. It never invents game decisions.

## Safety boundary

The dashboard is deliberately read-only. It cannot start a motor, switch mains
lighting, reset a rally, or change a rule. TeensyMaster remains the source of
truth and Motor/AC-Light Teensys retain their local safety timeouts.
