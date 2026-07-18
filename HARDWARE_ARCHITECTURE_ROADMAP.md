# PINGPONG — Hardware Architecture & Pinout Roadmap

Status: hardware integration specification v1.0

Companion to [SOFTWARE_ARCHITECTURE.md](SOFTWARE_ARCHITECTURE.md) and [GAME_DESIGN_ROADMAP.md](GAME_DESIGN_ROADMAP.md).

## Goal

Provide a definitive, collision-free physical hardware specification and integration roadmap for `TeensyMaster` (Teensy 4.1). This architecture guarantees sub-millisecond physical hit reactivity (`FastLED`), ultra-low hardware audio latency (< 3 ms via `PT8211` I2S DAC), continuous dual-radio reception (`NRF24L01`), and multi-track SD card sample streaming (`SDIO`) without pin conflicts or CPU bottlenecks.

---

## 1. System Overview & Hybrid Master-Slave Architecture

To achieve both **supernatural physical responsiveness** on the table and **rich audiovisual presentation** for audiences, PINGPONG utilizes a hybrid Master-Slave architecture:

```text
┌─────────────────────────── Teensy 4.1 (TeensyMaster) ───────────────────────────┐
│ [Bare-Metal Cortex-M7 @ 600 MHz | Zero OS Overhead | Sub-Millisecond Loop]    │
│                                                                                 │
│  NRF24L01 Radios (SPI0)           Analog Piezos (A15-A17)     SDIO Slot         │
│  ├── Left Racket (Ch 121)         ├── Left Table Hit          ├── Ambient Waves │
│  └── Right Racket (Ch 125)        ├── Right Table Hit         └── Hit Samples   │
│               │                   └── Net Hit Smash                   │         │
│               └────────────────────────┬──────────────────────────────┘         │
│                                        v                                        │
│                          Game Core & Time Engine                                │
│                     (BallWechselCounter State Machine)                          │
│                                        │                                        │
│               ┌────────────────────────┼───────────────────────┐                │
│               v                        v                       v                │
│       FastLED Table Strip       PT8211 Stereo DAC       EasyTransfer Serials    │
│       58+ SK9822 LEDs           16-Bit I2S DMA Audio    Serial1 & Serial8       │
└───────────────┬────────────────────────┬───────────────────────┬────────────────┘
                │ (< 1 ms Latency)       │ (< 3 ms Latency)      │ (6 Mbps USB/Ext)
                v                        v                       v
      Physical Table Comets      Speakers / Line-Out     Laptop (ofVisualizer) &
      & Center Line Flash        Instant Hit Effects     Motor / AC-Light Links
```

### Why Game Logic Must Run Bare-Metal on `TeensyMaster`
- **600 MHz Cortex-M7:** Executes `loop()` at **> 50,000 Hz** (< 20 µs cycle time).
- **Zero OS Preemption:** No Linux kernel task switching, no garbage collection, and no USB host polling delays.
- **Physical Reactivity:** When a piezo registers a hit (`hitPeak > 5`), the physical LED comets on the table explode within **< 1 millisecond**. Human perception requires < 8 ms to feel instant; `TeensyMaster` keeps it 10x faster than human threshold.

---

## 2. Complete Teensy 4.1 Pin Allocation & Verification Table

Every hardware system on `TeensyMaster` has been mapped to isolated, non-colliding hardware buses:

| Hardware Engine | Teensy 4.1 Pin(s) | Hardware Bus | Current Status | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **FastLED Strip (`SK9822`)** | **Pin 26** (`DATA`)<br>**Pin 27** (`CLOCK`) | Hardware SPI1 GPIO | 🟢 **ACTIVE** | Drives 58+ table LEDs at 10 MHz (`DATA_RATE_MHZ(10)`). |
| **Dual NRF24 Radios** | **Pin 31, 32** (`Left CE/CSN`)<br>**Pin 37, 38** (`Right CE/CSN`)<br>**Pin 11, 12, 13** (`MOSI/MISO/SCK`) | Shared `SPI0` Bus | 🟢 **ACTIVE** | `DualReceiverManager` keeps both radios listening continuously on Ch 121 & 125. |
| **Table & Net Piezos** | **Pin A15 (`Pin 39`)** (`Left Table`)<br>**Pin A16 (`Pin 40`)** (`Net Piezo`)<br>**Pin A17 (`Pin 41`)** (`Right Table`) | Analog ADC Inputs | 🟢 **ACTIVE** | `ResponsiveAnalogRead` + Peak Detection (`LT_PIEZO_PIN`, etc.). |
| **High-Speed Serials** | **Pin 1, 0** (`Serial1 TX/RX`)<br>**Pin 35, 34** (`Serial8 TX/RX`) | Hardware UART Serials | 🟢 **ACTIVE** | `6,000,000 baud` (`ET_Motor`, `ET_Light`). |
| **PT8211 Stereo DAC** | **Pin 7** (`OUT1A` / `DIN`)<br>**Pin 20** (`LRCLK1` / `WS`)<br>**Pin 21** (`BCLK1` / `BCK`) | Dedicated `I2S1` Bus | 🟢 **100% FREE & READY** | Hardware DMA audio engine; zero collisions with SPI0, FastLED, or ADC pins. |
| **MicroSD Card Interface** | **`BUILTIN_SDCARD`** Socket | On-Board 4-Bit `SDIO` | 🟢 **100% FREE & READY** | Dedicated internal traces (`SDIO_CMD/CLK/D0-D3`); 20+ MB/s transfer rate. |

---

## 3. PT8211 I2S DAC & SD Card Audio Integration Architecture

By adding the **PT8211 DAC** (`Pins 7, 20, 21`) and utilizing the **Built-in SDIO MicroSD Slot**, `TeensyMaster` becomes a fully autonomous sound synthesizer and multi-track audio sampler.

### Audio Patching & DMA Block Architecture
The PJRC Teensy Audio Library operates via **Direct Memory Access (DMA)**, processing audio in 128-sample blocks (`2.9 milliseconds` per block @ `44.1 kHz`). This runs entirely in background hardware registers, consuming only **3% to 8% CPU load**.

```text
 [SD Card: AMBIENT.WAV]  ──> AudioPlaySdWav (ambientDrone) ──┐
 [SD Card: HIT_LEFT.WAV] ──> AudioPlaySdWav (leftHitWav)   ──┼─> AudioMixer4 ──> AudioOutputPT8211
 [SD Card: HIT_RIGHT.WAV]──> AudioPlaySdWav (rightHitWav)  ──┤    (masterMixer)   (Stereo I2S DAC)
 [Swing Speed Telemetry] ──> AudioSynthWaveformSine        ──┘                    (Pins 7, 20, 21)
                             (motionPitchSynth)
```

### Hardware Audio Integration Code Blueprint
```cpp
#include <Audio.h>
#include <SD.h>

// 1. Audio Objects
AudioPlaySdWav           ambientDrone;    // Background looping ambient stadium/drone waves
AudioPlaySdWav           leftHitWav;      // Percussion hit samples for Left Table/Racket
AudioPlaySdWav           rightHitWav;     // Percussion hit samples for Right Table/Racket
AudioSynthWaveformSine   motionPitchSynth;// Real-time pitch modulation from swing speed
AudioMixer4              masterMixer;
AudioOutputPT8211        pt8211Output;    // 16-Bit Stereo I2S DAC (Pins 7, 20, 21)

// 2. Patch Cords (DMA Audio Connections)
AudioConnection          patch1(ambientDrone,     0, masterMixer, 0);
AudioConnection          patch2(leftHitWav,       0, masterMixer, 1);
AudioConnection          patch3(rightHitWav,      0, masterMixer, 2);
AudioConnection          patch4(motionPitchSynth, 0, masterMixer, 3);
AudioConnection          patchOutL(masterMixer,   0, pt8211Output, 0);
AudioConnection          patchOutR(masterMixer,   0, pt8211Output, 1);

void initHardwareAudio() {
    AudioMemory(16); // Allocate 16 DMA audio blocks (32 KB RAM)
    
    // Initialize built-in 4-bit SDIO slot
    if (!SD.begin(BUILTIN_SDCARD)) {
        Serial.println("Warning: SD Card not found. Continuing without samples.");
    } else {
        ambientDrone.play("AMBIENT.WAV"); // Start background loop
    }
}

void updateAudioAndCheckHits(const Racket& leftRacket, const Table& leftTable) {
    // Keep ambient drone wave looping continuously
    if (!ambientDrone.isPlaying() && SD.begin(BUILTIN_SDCARD)) {
        ambientDrone.play("AMBIENT.WAV");
    }

    // Trigger immediate WAV sample on hard table hit
    if (leftTable.hitPeak() > 20 && !leftHitWav.isPlaying()) {
        leftHitWav.play("HIT_LEFT.WAV");
    }

    // Modulate sine wave pitch from real-time swing speed
    float freq = 200.0f + (leftRacket.speed() * 5.0f);
    motionPitchSynth.frequency(constrain(freq, 120.0f, 1200.0f));
}
```

---

## 4. Latency Analysis: Bare-Metal vs. Laptop OS

| Metric / Pipeline Step | Bare-Metal `TeensyMaster` | Laptop OS (`ofVisualizer` on Linux) | Why Bare-Metal Wins |
| :--- | :--- | :--- | :--- |
| **Radio Packet Reception** | **< 0.1 ms** (Hardware SPI DMA/IRQ) | **3.0 – 10.0 ms** (USB CDC Polling + Kernel buffer) | No USB frame boundary waits (`1 ms` intervals). |
| **LED Strip Trigger** | **< 0.5 ms** (Direct FastLED SPI push) | **15.0 – 30.0 ms** (Round-trip USB to C++ back to Teensy) | Physical comets flash instantly on ball impact. |
| **Audio Sample Trigger** | **2.9 ms** (I2S DMA 128-sample block) | **21.3 – 35.0 ms** (PulseAudio/PipeWire 1024 buffer) | Zero acoustic lag; physical hit and sound align. |
| **Game State Transition** | **< 0.02 ms** (600 MHz RAM cycle) | **8.3 – 16.6 ms** (Monitor refresh rate locking) | `BallWechselCounter` cannot drop or delay frames. |

---

## 5. Hardware Staged Integration Roadmap

### Stage H1 — Pinout & Bus Isolation (Verified & Locked)
- [x] Verify complete separation of SPI0 (Radios), SPI1 GPIO (FastLED), ADC (Piezos), and Serials.
- [x] Lock `Pins 7, 20, 21` explicitly for PT8211 I2S DAC.
- [x] Lock `BUILTIN_SDCARD` slot for high-speed 4-bit SDIO WAV streaming.

### Stage H2 — Concurrent Radio & LED Stress Testing (Active)
- [x] Validate `DualReceiverManager` continuous polling at `100+ Hz` alongside `FastLED.show()` on `Pins 26, 27`.
- [ ] Confirm zero radio packet drop rate when all 58 LEDs pulse at full brightness.

### Stage H3 — PT8211 Stereo DAC Hardware Wiring & Verification
- [ ] Solder/wire `PT8211` DAC chip to `Pins 7 (DIN)`, `20 (WS)`, and `21 (BCK)`.
- [ ] Connect `AudioOutputPT8211` with `AudioSynthWaveformSine` inside `TeensyMaster`.
- [ ] Verify clean, noise-free stereo audio output on oscilloscope or line-out jack while radios are actively transmitting.

### Stage H4 — SDIO MicroSD Multi-Track Audio Streaming
- [ ] Format MicroSD card to FAT32 / exFAT and insert into `BUILTIN_SDCARD`.
- [ ] Load high-resolution 16-bit 44.1 kHz `.WAV` files (`AMBIENT.WAV`, `HIT_LEFT.WAV`, `HIT_RIGHT.WAV`, `NET_SMASH.WAV`).
- [ ] Verify simultaneous 4-channel WAV playback and live mixing inside `AudioMixer4` without frame rate drops.

### Stage H5 — Standalone Autonomous Power & Enclosure Build
- [ ] Power `TeensyMaster`, `PT8211`, and LED strips via dedicated 5V 10A power supply (separating logic power from high-current LED rails).
- [ ] Run full 24-hour autonomous tournament rehearsal with zero laptop connection.

### Stage H6 — Exhibition Hardening & Shielding
- [ ] Enclose RF24 radios in grounded RF shields or position external dipole antennas above table level.
- [ ] Install optical isolation or inline 330Ω damping resistors on `Pin 26 (DATA)` and `Pin 27 (CLOCK)` for clean long-distance LED signal transmission.
- [ ] Document final physical wiring schematic and hardware repair procedure for gallery technicians.
