# PINGPONG — Scalable Software Architecture

Status: implementation architecture draft v0.2

Implementation status: Stage 0 is implemented in the current worktree and all
three firmware targets build. It still requires a connected-hardware test before
being considered exhibition-ready.

Companion to [GAME_DESIGN_ROADMAP.md](GAME_DESIGN_ROADMAP.md).

## Goal

Allow rules, sound, lights, motors, and sensors to evolve without changes in one
area breaking the others. Keep hit response immediate even when the represented
world becomes extremely slow.

The smallest useful architecture is:

```text
SENSES -> EVENTS -> REFEREE -> RELATIONSHIP -> WORLD FRAME -> INSTRUMENTS
```

In artistic language:

- sensors are the senses;
- `GameEngine` is the referee;
- `ConnectionEngine` is the dramaturg and memory of the lovers;
- `WorldFrame` is the shared score;
- PlugData, LEDs, motors, and AC lights are instruments performing that score.

## Core rule: one source of truth

TeensyMaster owns all game and relationship truth:

- current game phase;
- expected next contact;
- rally exchange count;
- valid hit and mistake events;
- persistent connection level;
- progress toward the next level;
- world speed, noise gain, voice gains, and shared rhythm;
- hardware health and paused state.

No output device decides rules. PlugData, TeensyMotor, and TeensyAcLight only
render commands from TeensyMaster and protect their own hardware locally.

The browser-based Relationship Mirror is another read-only renderer. It receives
20 Hz USB telemetry for observation and debugging but cannot modify game state or
drive physical outputs. Its implementation and startup instructions are in
[`webui/`](webui/README.md).

## System diagram

```text
┌──────────────────────────── TeensyMaster ────────────────────────────┐
│                                                                      │
│  Racket radios             Table piezos                              │
│       │                         │                                    │
│       └────────────┬────────────┘                                    │
│                    v                                                 │
│             InputEvent collector                                    │
│       source / timestamp / strength / freshness                     │
│                    │                                                 │
│                    v                                                 │
│          GameEngine — the referee                                   │
│ phase / expected input / exchange / mistake                         │
│                    │                                                 │
│                    v                                                 │
│      ConnectionEngine — the relationship                            │
│ level / progress / world speed / noise / voices / rhythm            │
│                    │                                                 │
│                    v                                                 │
│               WorldFrame builder                                    │
│                    │                                                 │
│       ┌────────────┼───────────────┬─────────────┐                   │
│       v            v               v             v                   │
│ LED renderer   MIDI renderer   Motor link     Light link             │
└────────────────────┼───────────────┼─────────────┼───────────────────┘
                     │               │             │
                     v               v             v
                  PlugData      TeensyMotor   TeensyAcLight
```

## Module responsibilities

### Input layer

Existing racket and table classes remain the hardware-facing layer. A small
collector converts hit edges into fixed-size events:

```cpp
enum class InputSource : uint8_t {
    LeftRacket,
    RightRacket,
    LeftTable,
    RightTable,
};

struct InputEvent {
    uint32_t timeMs;
    InputSource source;
    uint16_t strength;
};
```

At most four sensor events can be produced in one loop, so the first version can
use a fixed four-element event batch. No heap allocation is needed. Introduce a
ring buffer only if IRQ-driven inputs later require it.

The input layer reports facts. It never decides whether an event is legal.

### GameEngine — rules only

`GameEngine` receives one `InputEvent` at a time and owns:

- Ready, Serve, Rally, and Fault phases;
- which sensor is expected next;
- valid-contact and completed-exchange pulses;
- rally timeout;
- mistake reason;
- current-rally exchange count.

It does not know about MIDI, LEDs, motors, noise, or connection levels.

The old `BallWechselCounter` is historical evidence for the rules, not the new
implementation. Its twenty states can become a small phase plus an
`expectedNext` value.

### ConnectionEngine — poetic progression only

`ConnectionEngine` receives meaningful game pulses:

```text
valid contact
completed exchange
player mistake
technical pause/resume
manual reset
```

It owns:

- connection level 0–5;
- progress toward the next level;
- one-level setback after a player mistake;
- full reset after long inactivity or manual reset;
- smooth transitions between level presets;
- world speed;
- ambient-noise gain;
- left/right player-voice gain;
- shared rally tempo and shadow synchronization;
- increasing player influence over the shared relationship body.

A hardware failure pauses this engine. It does not reduce the lovers' connection
level.

### LevelPreset — the artist's tuning surface

The six levels are data, not hard-coded behavior:

```cpp
struct LevelPreset {
    uint8_t exchangesToReach;
    uint16_t worldSpeed;
    uint8_t noiseGain;
    uint8_t leftVoiceGain;
    uint8_t rightVoiceGain;
    uint8_t shadowSync;
    uint8_t playerInfluence;
    uint16_t transitionMs;
};
```

Changing the composition during rehearsal should mean editing or loading these
presets, not rewriting the game rules.

### Renderers — outputs only

Each renderer receives the same read-only state:

- `LedRenderer` performs the shared heart/clock/flower animation;
- `MidiRenderer` sends two lover voices and relationship controls to PlugData;
- `MotorLink` sends world speed and gestures to TeensyMotor;
- `LightLink` sends level, speed, brightness, and shadow rhythm to
  TeensyAcLight;
- `Diagnostics` prints a low-rate human-readable state report.

A renderer must never mutate `GameEngine` or `ConnectionEngine`.

## One shared communication contract

The current Master frame begins with `leftRacketHit, rightRacketHit`, while the
Motor and AC-Light copies begin with `rightRacketHit, leftRacketHit`. EasyTransfer
copies raw bytes, so these meanings can be swapped. The fields named speed are
also inconsistent: the left field currently carries roll while the right field
carries rotational speed.

Create one shared protocol header and include it from every PlatformIO project.
Never copy the struct by hand again.

```cpp
constexpr uint8_t protocolVersion = 1;

struct __attribute__((packed)) WorldFrame {
    uint8_t version;
    uint16_t frameSequence;
    uint8_t systemStatus;
    uint8_t gamePhase;
    uint8_t connectionLevel;
    uint8_t levelProgress;

    uint16_t worldSpeed;
    uint16_t rallyTempoMs;
    uint8_t noiseGain;
    uint8_t leftVoiceGain;
    uint8_t rightVoiceGain;
    uint8_t shadowSync;
    uint8_t playerInfluence;

    uint8_t leftRacketHitCount;
    uint8_t rightRacketHitCount;
    uint8_t leftTableHitCount;
    uint8_t rightTableHitCount;
    uint8_t exchangeCount;
    uint8_t mistakeCount;

    int8_t leftRoll;
    int8_t rightRoll;
    uint8_t leftSwing;
    uint8_t rightSwing;
};
```

Use a compile-time size check and increment `protocolVersion` whenever the wire
layout changes. A receiver that sees the wrong version enters its safe state
instead of interpreting incorrect bytes.

Use wrapping event counters instead of one-loop booleans. If a serial frame is
lost, a peripheral still observes that the count changed and handles the event
once.

Send the frame at a fixed rate such as 100 Hz. Radio sensing and hit processing
continue as fast as possible; serial outputs do not need one frame per CPU loop.
An immediate racket or table event can still reach the local LED and USB MIDI
renderers in the same Master loop. The 100 Hz frame is the synchronized state and
heartbeat for remote Motor and AC-Light devices; it does not slow the inputs.

## Deterministic Master update order

Every Master loop follows the same order:

```text
1. SENSE      poll radios, rackets, and tables
2. COLLECT    create timestamped input events
3. REFEREE    process events and rule timeout
4. RELATION   apply progress, setback, pause, and smooth transitions
5. COMPOSE    build one immutable WorldFrame
6. RENDER     LEDs, USB MIDI, Motor link, Light link, diagnostics
```

This order keeps hit-to-sound and hit-to-light latency short at every connection
level. Artistic time slows; the control system does not.

## PlugData boundary

PlugData owns sound composition, not game truth. TeensyMaster sends:

- immediate hit notes and strengths;
- left/right motion, orientation, and pressure controls;
- connection level and progress;
- world speed;
- ambient-noise gain;
- left/right voice gains;
- shared rally tempo;
- mistake and reset events.

PlugData may transform these into any sound, but it does not independently count
rallies or decide levels. Restarting the patch therefore cannot corrupt the game.

## Fault containment

Every device protects itself.

### Master

- Track the age of the latest packet from each racket.
- A stale racket pauses the relationship without a player setback.
- Continue sending a hardware-paused WorldFrame.
- Show a distinct diagnostic light gesture.

### Motor Teensy

- Clamp requested speed to calibrated mechanical limits.
- If no valid WorldFrame arrives for about 500 ms, decelerate safely to idle.
- Reject unknown protocol versions.
- Never depend on Master for the final safety limit.

### AC-Light Teensy

- Remove USB `while (!Serial)` startup waits.
- Use non-blocking animation; never use `delay()` in the active loop.
- On communication timeout, move to a safe static light state.
- Clamp brightness and rate locally.

### PlugData

- On MIDI loss, fade to silence or a safe ambient state.
- Never produce uncontrolled full-scale output after reconnection.
- Keep a master limiter in the final audio path.

## Test seams

`GameEngine` and `ConnectionEngine` should be ordinary C++ without `Arduino.h`.
Time enters through function arguments. This allows PlatformIO native tests on a
computer.

Tests should script sequences such as:

```text
valid left serve
valid right serve
long alternating rally
wrong table
wrong racket
simultaneous contradictory contacts
rally timeout
mistake at Level 0
mistake at Level 4 -> Level 3
hardware pause -> no setback
manual full reset
```

Renderers can be tested with saved WorldFrames. Hardware rehearsals then verify
electrical behavior instead of rediscovering rule bugs.

## Repository structure

```text
pingpong/
├── GAME_DESIGN_ROADMAP.md
├── SOFTWARE_ARCHITECTURE.md
├── lib_shared/
│   ├── PingPongProtocol/     one WorldFrame definition
│   └── GameCore/             GameEngine, ConnectionEngine, presets
├── TeensyMaster/
│   └── src/
│       ├── input/            collector and health monitor
│       ├── game/             Master adapters for GameCore
│       ├── output/           LED, MIDI, Motor, Light renderers
│       └── Racket/ Table/    existing hardware layer
├── TeensyMotor/              safe local motor renderer
├── TeensyAcLight/            safe local light renderer
└── test/                     native scripted game tests
```

Do not reorganize the whole repository first. Create shared code only when its
first consumer is migrated, so the installation remains buildable throughout.

## Staged migration without breaking the artwork

### Stage 0 — Stabilize today's links (implemented; hardware test pending)

- Create the shared protocol with the current fields in one correct order.
- Add a version byte and fixed 100 Hz send rate.
- Rename roll and speed fields honestly.
- Remove blocking USB waits and production delays.
- Add communication timeout and local safety limits to peripherals.

Result: current behavior remains, but left/right data and power-cycle recovery
become trustworthy.

### Stage 1 — Trustworthy events

- Add receiver freshness timestamps and hardware health.
- Add the fixed-size event collector.
- Print one diagnostic event line with source, time, strength, and packet rate.

Result: every physical contact can be verified before rules are introduced.

### Stage 2 — GameEngine in shadow mode

- Implement and host-test the new referee.
- Run it on Master for diagnostics only.
- Keep all existing outputs unchanged.

Result: real players can prove the rules without risking the installation.

### Stage 3 — ConnectionEngine and level simulation

- Implement level presets, progress, one-level setback, and full reset.
- Add simulated events so the complete arc can be rehearsed without a table.
- Apply world speed to the Master LED animation first.

Result: the poetic structure works before every device is connected.

### Stage 4 — WorldFrame renderers

- Extend the shared protocol and increment its version.
- Connect motor and AC-light renderers one at a time.
- Test cable removal, stale data, and recovery.

Result: outputs decelerate together but fail independently and safely.

### Stage 5 — PlugData composition

- Build two lover voices, interference noise, and the shared relationship body.
- Map all relationship controls from Master.
- Confirm that immediate hits remain immediate at the slowest level.

Result: the complete artistic arc is playable and tunable.

### Stage 6 — Exhibition hardening

- Add watchdog recovery, startup self-test, and production diagnostics switch.
- Save exact firmware and PlugData versions.
- Document flashing, wiring, calibration, startup, and reset procedures.

Result: another technician can operate and recover the work without editing code.

## Scalability test

A future feature is correctly isolated when:

- adding a projector requires one new renderer, not a rule change;
- changing rally rules requires only GameEngine and its tests;
- changing level thresholds requires only presets;
- changing sound requires only PlugData/MidiRenderer;
- changing a motor gesture requires only TeensyMotor;
- losing one output does not stop sensors, rules, or the other outputs.

That is what “scalable without breaking the game” means for this installation.
