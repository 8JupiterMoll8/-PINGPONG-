#pragma once

#include <Arduino.h>
#include <cmath>
#include <cstdint>

struct RacketTelemetry {
    float gx{0.0F};
    float gy{0.0F};
    float gz{0.0F};
    float ax{0.0F};
    float ay{0.0F};
    float az{0.0F};
    int32_t piezo{0};
    int32_t pressure{0};
    float roll{0.0F};
    float pitch{0.0F};
    float yaw{0.0F};
    float speed{0.0F};
    uint8_t hitCount{0};
    uint8_t hitPeak{0};
};

struct RadioTelemetry {
    uint32_t leftPackets{0};
    uint32_t rightPackets{0};
    uint32_t leftAgeMs{UINT32_MAX};
    uint32_t rightAgeMs{UINT32_MAX};
    bool leftFresh{false};
    bool rightFresh{false};
};

struct TableTelemetry {
    uint8_t leftHitCount{0};
    uint8_t rightHitCount{0};
    uint8_t netHitCount{0};
    uint8_t leftHitPeak{0};
    uint8_t rightHitPeak{0};
    uint8_t netHitPeak{0};
};

// Reserved now so the dashboard contract does not need to change when the
// referee and connection engines are introduced.
struct GameTelemetry {
    uint8_t phase{0};
    uint8_t expectedInput{0};
    uint16_t rallyCount{0};
    uint8_t connectionLevel{0};
    uint8_t levelProgress{0};
};

struct OutputTelemetry {
    int32_t motorTarget{0};
    uint8_t lightPulseCount{0};
    uint8_t ledMode{0};
    uint8_t soundMode{0};
};

struct WebTelemetryFrame {
    uint32_t timestampMs{0};
    uint16_t frameSequence{0};
    uint8_t systemStatus{0};
    RadioTelemetry radio{};
    RacketTelemetry left{};
    RacketTelemetry right{};
    TableTelemetry table{};
    GameTelemetry game{};
    OutputTelemetry outputs{};
};

class WebTelemetry {
public:
    explicit WebTelemetry(Print& output) : output_(output) {}

    void send(const WebTelemetryFrame& frame)
    {
        output_.print(F("PP:{\"v\":1,\"ms\":"));
        output_.print(frame.timestampMs);
        output_.print(F(",\"seq\":"));
        output_.print(frame.frameSequence);
        output_.print(F(",\"status\":"));
        output_.print(frame.systemStatus);

        output_.print(F(",\"radio\":{\"leftPackets\":"));
        output_.print(frame.radio.leftPackets);
        output_.print(F(",\"rightPackets\":"));
        output_.print(frame.radio.rightPackets);
        output_.print(F(",\"leftAge\":"));
        output_.print(frame.radio.leftAgeMs);
        output_.print(F(",\"rightAge\":"));
        output_.print(frame.radio.rightAgeMs);
        output_.print(F(",\"leftFresh\":"));
        output_.print(frame.radio.leftFresh ? 1 : 0);
        output_.print(F(",\"rightFresh\":"));
        output_.print(frame.radio.rightFresh ? 1 : 0);
        output_.print('}');

        printRacket(F("left"), frame.left);
        printRacket(F("right"), frame.right);

        output_.print(F(",\"table\":{\"leftHit\":"));
        output_.print(frame.table.leftHitCount);
        output_.print(F(",\"rightHit\":"));
        output_.print(frame.table.rightHitCount);
        output_.print(F(",\"netHit\":"));
        output_.print(frame.table.netHitCount);
        output_.print(F(",\"leftPeak\":"));
        output_.print(frame.table.leftHitPeak);
        output_.print(F(",\"rightPeak\":"));
        output_.print(frame.table.rightHitPeak);
        output_.print(F(",\"netPeak\":"));
        output_.print(frame.table.netHitPeak);
        output_.print('}');

        output_.print(F(",\"game\":{\"phase\":"));
        output_.print(frame.game.phase);
        output_.print(F(",\"expected\":"));
        output_.print(frame.game.expectedInput);
        output_.print(F(",\"rally\":"));
        output_.print(frame.game.rallyCount);
        output_.print(F(",\"level\":"));
        output_.print(frame.game.connectionLevel);
        output_.print(F(",\"progress\":"));
        output_.print(frame.game.levelProgress);
        output_.print('}');

        output_.print(F(",\"outputs\":{\"motorTarget\":"));
        output_.print(frame.outputs.motorTarget);
        output_.print(F(",\"lightPulse\":"));
        output_.print(frame.outputs.lightPulseCount);
        output_.print(F(",\"ledMode\":"));
        output_.print(frame.outputs.ledMode);
        output_.print(F(",\"soundMode\":"));
        output_.print(frame.outputs.soundMode);
        output_.println(F("}}"));
    }

private:
    Print& output_;

    void printRacket(const __FlashStringHelper* name, const RacketTelemetry& racket)
    {
        output_.print(F(",\""));
        output_.print(name);
        output_.print(F("\":{\"gx\":"));
        printFloat(racket.gx);
        output_.print(F(",\"gy\":"));
        printFloat(racket.gy);
        output_.print(F(",\"gz\":"));
        printFloat(racket.gz);
        output_.print(F(",\"ax\":"));
        printFloat(racket.ax);
        output_.print(F(",\"ay\":"));
        printFloat(racket.ay);
        output_.print(F(",\"az\":"));
        printFloat(racket.az);
        output_.print(F(",\"piezo\":"));
        output_.print(racket.piezo);
        output_.print(F(",\"pressure\":"));
        output_.print(racket.pressure);
        output_.print(F(",\"roll\":"));
        printFloat(racket.roll);
        output_.print(F(",\"pitch\":"));
        printFloat(racket.pitch);
        output_.print(F(",\"yaw\":"));
        printFloat(racket.yaw);
        output_.print(F(",\"speed\":"));
        printFloat(racket.speed);
        output_.print(F(",\"hit\":"));
        output_.print(racket.hitCount);
        output_.print(F(",\"peak\":"));
        output_.print(racket.hitPeak);
        output_.print('}');
    }

    void printFloat(float value)
    {
        if (std::isfinite(value)) {
            output_.print(value, 2);
        } else {
            output_.print('0');
        }
    }
};
