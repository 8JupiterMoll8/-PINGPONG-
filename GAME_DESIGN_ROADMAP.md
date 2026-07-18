# PINGPONG — Time Deceleration Game

Status: artist-confirmed design draft v0.4

Technical implementation boundaries and migration are specified in
[SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md).

This document separates behavior already visible in the old code from proposed
behavior that still needs artistic confirmation.

## Artistic vision

PINGPONG is a cooperative physical table-tennis game, audiovisual instrument,
and poetic metaphor for two lovers finding each other again. Noise represents
the interference and disruption that prevents them from hearing one another.
The rally is their conversation. Every successful return is an act of listening
and repair.

The installation begins in hyper-time. Knight Rider lights, motors, and sonic
activity move so quickly that individual motion can no longer be perceived. The
light appears almost frozen or continuous, shadows disappear into speed, and
noise masks the lovers. This is apparent stillness caused by excess motion.

The lovers must begin the rally to decelerate this world. As the rally survives,
the installation moves through levels of connection. Machine time slows enough
for pulses, movement, and shadows to appear. Ambient-noise volume falls away,
and each player's individual melody becomes clearer. The two light/shadow
movements progressively find a shared rhythm. At the deepest level, the machine
is calm and only the players' voices remain. They retain their freedom and
individual melodies, but become audible together.

The work therefore travels between two different forms of stillness:

```text
BEGINNING: movement too fast to perceive -> apparent frozen light + noise
ENDING:    movement deeply slowed       -> visible shadows + human calm
```

The central heart/clock/flower/light sculpture shown in the original drawing is
not owned by either lover. It is a shared third body: the relationship itself.
Both lovers act upon it, and it carries their accumulated connection, noise,
speed, light, shadow, and memory.

The physical sensors and rule detection must always remain real-time so that
slowing the artwork never makes it miss a hit.

The central dramatic cycle is:

```text
DISTANCE/NOISE -> SERVE -> RALLY -> LISTENING -> REUNION/FREEDOM
                       \-> MISTAKE -> ONE LEVEL BACK -> TRY AGAIN
```

Impacts should remain immediate. Their visual, sonic, and mechanical aftermath
can unfold in increasingly slow time. A mistake does not erase the relationship:
it creates distance, but the players can reconnect.

## Reconstructed rules from the existing code

### Start and serve

Either racket can begin a game.

Left serve:

```text
left racket -> left table -> right table -> right racket
```

Right serve:

```text
right racket -> right table -> left table -> left racket
```

### Rally

After the serve, the expected sequence alternates:

```text
right racket -> left table -> left racket -> right table -> right racket -> ...
```

The mirrored sequence applies when the rally begins from the other side. One
completed return cycle increments the rally/exchange count.

### Existing reset rule

When a sensor fires out of the expected order, the old state machine enters a
`DOPPEL_FEHLER` (double fault) state and returns to idle.

The old code does not yet define whether a fault resets only the current rally,
the rally count, a larger game score, or all of them. It also contains an empty
`FINISH_GAME` state.

## Additional artistic language recovered from TeensyMaster/src

The rackets were designed as expressive instruments, not only hit switches:

- Racket swing continuously controls two alternating MIDI gains.
- Grip pressure creates a charging behavior.
- Holding pressure activates roll, pitch, and yaw as MIDI controls.
- Racket impact strength controls comet speed, width, fade, and MIDI velocity.
- Table impact strength creates a full-strip white flash and MIDI note.
- Two opposing comets were intended to travel across the shared strip, collide,
  change color, and reverse direction.

This suggests three simultaneous artistic layers:

```text
BODY LAYER   continuous swing, orientation, and pressure
BALL LAYER   discrete racket/table impacts and moving comets
TIME LAYER   rally rules progressively decelerate the whole environment
```

The first two layers exist as disconnected prototypes. The rally manager, comet
updates, and comet collision are commented out in the active Master loop. The
time layer does not yet exist. Finishing the work means composing these layers
under one game and time engine, not merely reconnecting every old class.

## Confirmed game constitution

- The players cooperate to create the longest possible rally.
- The work progresses through distinct levels of deceleration and listening.
- Everything mechanical and atmospheric becomes slower as connection grows.
- The opening is super-fast, visually fused, noisy, and machine-dominated.
- Deceleration reveals movement and shadows that were hidden by speed.
- Ambient noise is gradually removed rather than merely becoming slower.
- Each player has an individual melody derived from their movement.
- The two melodies become increasingly clear and able to coexist.
- The two light/shadow movements progressively synchronize with the shared
  rhythm created by the rally.
- At the deepest level, the machine recedes and only the players are heard.
- A mistake should probably move the relationship one level backward instead of
  returning all progress to zero. This remains a tuning decision to confirm in
  rehearsal.

## Connection levels — working composition

The earlier continuous exponential curve is replaced by a discrete level
composition. Numerical values and exchange thresholds are starting points for
rehearsal, not final rules.

| Level | Meaning | Exchanges to reach | Motor and Knight Riders | Shadows/rhythm | Ambient noise | Player melodies |
|---:|---|---:|---|---|---:|---:|
| 0 | Distance | Start | Safe hyper-speed; appears fused/frozen | Hidden and unrelated | 100% | 10%, fragmented |
| 1 | Recognition | 2 | Very fast | First pulses become visible | 80% | 30% |
| 2 | Listening | 5 | Fast but clearly moving | Separate moving shadows | 55% | 50% |
| 3 | Dialogue | 9 | Human-perceivable pace | Call and response | 30% | 75% |
| 4 | Reunion | 14 | Slow | Rhythms converge | 10% | 100% |
| 5 | Freedom | 20 | Very slow and calm | Shared rhythm; both shadows present | 0% | Only the two players |

Transitions between levels should glide smoothly. The levels are audible and
visible chapters, not abrupt technical switches.

### The two player melodies

Each racket should remain an independent voice:

- orientation selects or bends pitch and melodic direction;
- swing intensity controls energy, articulation, or timbre;
- grip pressure sustains, opens, or charges the voice;
- impact creates an immediate note or phrase boundary.

Both voices can inhabit a shared harmonic field so they can meet without one
voice being forced to copy the other. The artistic goal is harmony through
freedom, not uniformity.

### Perceptual movement and shadows

The opening visual must not look like an ordinary fast chase animation. It
should cross the threshold where speed visually fuses into an apparently fixed
field. Deceleration then reveals the animation as motion.

The central sculpture may have two wings or directions, but it remains one
organism. Each lover contributes an individual gesture and melody to this shared
body. As connection increases, its initially chaotic or fused movement becomes
a legible shared rhythm derived from successful ball contacts. This does not
require the lovers to make identical gestures. Their exchange creates the clock
of the relationship.

The control model is:

```text
early levels: machine speed dominates; player influence is difficult to hear/see
late levels:  machine recedes; both players shape one shared relational body
```

All motor speeds must remain within calibrated mechanical limits. Visual
hyper-speed can be represented by LED phase and sonic density without driving a
physical mechanism beyond a safe speed. Visible flashing frequencies must also
be tested for audience safety.

## What slows and what never slows

### Always real-time

- Radio reception
- Racket and table hit detection
- Rule validation
- Safety checks and disconnect detection
- Immediate attack of a hit sound or flash

### Controlled by connection level and `timeScale`

- LED comet velocity and decay
- Motor/physical clock speed
- AC-light animation intervals
- Musical tempo, envelopes, delay, and reverb time
- Ambient-noise gain, which approaches silence at the final level
- Left and right player-melody gain and clarity

## Reset conditions to design explicitly

Proposed setback and reset triggers:

1. A hit arrives from the wrong racket or table.
2. Two contradictory hits arrive in the same event window.
3. The expected next hit does not arrive before a configurable timeout.
4. A manual reset control is activated.
5. A long period of inactivity ends the complete experience.

A normal player mistake ends the current rally, resets its exchange counter,
and moves the persistent connection level back by one. A new serve begins from
that slightly more distant state. Repeated mistakes can eventually return the
work to Level 0. This lets the relationship remember progress while still making
disruption perceptible.

A radio or hardware failure is not a failure by the lovers. It should pause the
game safely and show a technical diagnostic without reducing their connection
level.

All transitions must be non-blocking. Sensors continue running while the
artwork performs the fault or reconnection gesture. A full reset to Level 0 is
reserved for long inactivity, a manual reset, power cycling, or an artistically
defined ending.

## Target technical architecture

```text
Racket radios + table piezos
            |
            v
Timestamped input events (always real-time)
            |
            v
Game rule engine
  phase / expected input / rally exchanges / mistake
            |
            v
Connection and time engine
  level / world speed / noise / voices / shadow phase / setback
            |
            v
Output frame
  sound / LEDs / motor clock / AC lights
```

The rule engine owns game truth. The connection engine owns the poetic arc and
the time/noise/voice mix. Output devices receive the result; they do not
independently decide the rules.

## Sound architecture decision — PlugData-first hybrid

Use the Teensy Master for deterministic real-time responsibilities:

- receiving both rackets and table sensors;
- validating serve and rally rules;
- maintaining connection level and setback memory;
- controlling physical LEDs, motors, and safety;
- sending timestamped events and continuous controls over USB MIDI.

Use PlugData for the evolving composition:

- the initial dense interference/noise layer;
- one expressive voice for each lover;
- the shared resonant/harmonic body of the relationship;
- gradual noise removal and voice revelation;
- tempo, density, envelope, delay, and reverb deceleration;
- rehearsal control of all level transitions.

```text
LEFT LOVER  ---- individual MIDI voice ----\
                                             \
GAME/LEVEL  ---- noise + time + relation -----> PlugData -> sound system
                                             /
RIGHT LOVER ---- individual MIDI voice -----/
```

This is preferable during artistic development because PlugData makes the
composition visible, audible, and quickly adjustable. The Teensy Audio Library
is valuable when the final priority becomes a computer-free embedded system,
but it is less flexible for discovering granular noise, long effects, and the
changing relationship between two melodies.

For exhibition, choose between:

1. a dedicated mini-computer running the finished PlugData patch with automatic
   startup; or
2. a separate audio Teensy running a simplified final composition.

Do not initially place the complete audio synthesis on the existing Master
Teensy. Keeping game/safety control separate from sound experimentation reduces
risk and makes both systems easier to diagnose.

## Data protocol needed between Teensys

The current link only sends racket/table hit flags and two values named speed.
The completed game needs one shared, versioned frame used by Master, Motor, and
Light firmware:

```text
protocol version
game phase
expected next input
valid-hit pulse
fault/reset pulse
completed exchange count
connection level
progress toward next level
time scale
smoothed rally tempo
shadow synchronization
player influence
ambient-noise gain
left/right player-voice gain
left/right racket hit
left/right table hit
left/right movement values
```

One shared definition is important: the current Master and peripheral copies
place the left/right racket flags in a different order, which can swap their
meaning on the wire.

## Technical roadmap

### Milestone 1 — Lock the artistic constitution

Deliverable: one-page confirmed rule set.

- Preserve the confirmed cooperative goal: create the longest possible rally.
- Confirm whether levels advance by completed exchanges or every legal contact.
- Rehearse and confirm the one-level setback after a mistake.
- Define whether Freedom is held indefinitely or completes an arc.
- Compose the transformation from ambient noise to two player melodies.
- Define exactly what the physical clock/motor represents at each level.

Done when two people can read the rules and agree on every transition.

### Milestone 2 — Make sensing performance-safe

Deliverable: a diagnostic firmware mode with trustworthy sensor events.

- Finish simultaneous dual-racket reception.
- Add racket disconnect/stale-data detection.
- Calibrate four independent hit detectors.
- Remove blocking debug output and delays from production loops.
- Test both rackets and both table sensors simultaneously.
- Log event name, timestamp, strength, and radio packet rate.

Done when a rehearsal produces no unexplained missing or duplicate hit events.

### Milestone 3 — Replace the old rule state machine

Deliverable: a small deterministic `GameEngine`.

- Use explicit states: Ready, ServeLeft, ServeRight, Rally, Fault, Reset,
  Complete.
- Represent the expected next sensor directly instead of twenty named states.
- Process one timestamped event at a time.
- Define simultaneous-event priority and timeout behavior.
- Correct left/right wiring and add resettable counters.
- Keep a serial diagnostic view of state, expected input, and fault reason.

The existing `BallWechselCounter` should be treated as historical rule evidence,
not re-enabled unchanged: it is disabled, has side-order mistakes, and contains
an incomplete fault predicate and finish state.

Done when scripted hit sequences prove every legal and illegal transition.

### Milestone 4 — Build the connection and time engine

Deliverable: one global connection level with coordinated artistic parameters.

- Advance levels from confirmed rally thresholds.
- Derive time scale, motor speed, light speed, ambient-noise gain, and player
  voice gains from one level preset.
- Derive a stable shared rhythm from valid contact timing.
- Move left/right light phases gradually toward synchronization.
- Smooth changes without delaying hit detection.
- Implement one-level setback, full reset, and Freedom behavior.
- Make thresholds and level presets configurable for rehearsal tuning.

Done when a simulated rally travels through the complete poetic arc and mistakes
move backward exactly as intended.

### Milestone 5 — Integrate one output at a time

Deliverable: the same time state is perceptible across the installation.

Recommended order:

1. Serial visualization of game state and time scale.
2. Master LED strip.
3. PlugData patch with noise, two lover voices, shared relationship body, and
   dedicated level/time controls.
4. Motor/physical clock.
5. AC-light controller.

All animations must be non-blocking. Every device needs a communication timeout
that moves it to a safe state if Master data stops.

Done when all outputs decelerate together and a new hit is still perceived
immediately at the minimum time scale.

### Milestone 6 — Rehearsal calibration

Deliverable: saved installation presets.

- Tune sensor thresholds using real players and the actual room.
- Tune the deceleration curve and minimum scale artistically.
- Measure end-to-end hit-to-sound and hit-to-light latency.
- Test short, long, soft, hard, and simultaneous rallies.
- Test power cycling and deliberate radio disconnection.

Done when setup does not require source-code changes.

### Milestone 7 — Exhibition hardening

Deliverable: a recoverable installation build.

- Remove production USB wait loops and repetitive debug printing.
- Move radios to legal, site-tested 2.4 GHz channels.
- Add startup self-test and clear fault indicators.
- Add a physical reset/restart procedure.
- Save exact firmware versions and flashing instructions for every Teensy.
- Document wiring, power, spare hardware, and recovery steps.

Done when another technician can start, diagnose, and reset the work without its
author editing code.

## Minimum finishable artwork

The fastest meaningful finished version is:

1. Both RAW rackets and both table sensors produce reliable events.
2. A rewritten rule engine validates serve and rally order.
3. Completed exchanges advance through connection levels.
4. Each level slows LEDs/motor, removes noise, and reveals both melodies.
5. Wrong order or timeout ends the rally and moves one level backward.
6. Motor and AC-light integration follow after this core experience is stable.

This creates the complete artistic idea early, before spending time hardening
every peripheral.

## Open artistic decisions

1. Should a level advance after every valid contact or after complete exchanges?
2. How many exchanges should be needed for each level?
3. Does one mistake always move back exactly one level?
4. How long may the ball be absent before a rally is considered broken?
5. At Freedom, does the game continue indefinitely, hold for a ritual moment,
   or complete and begin again?
6. What musical rules let the two independent melodies become compatible while
   preserving each player's freedom?
7. How should each lover's individual movement contribute to the one shared
   heart/clock/flower body at Freedom?
8. What does the motorized clock physically do at Distance, Reunion, Freedom,
   mistake, and full reset?
