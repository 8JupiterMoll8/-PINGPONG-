
#pragma once

#include "PingPong.h"

/**
 * BallWechselCounter — legal ping-pong rally state machine
 *
 * Tracks the expected sensor sequence for left-serve and right-serve rallies:
 *   Left serve:  LRacket → LTable → RTable → RRacket → LTable → LRacket → RTable → RRacket → COUNT
 *   Right serve: RRacket → RTable → LTable → LRacket → RTable → RRacket → LTable → LRacket → COUNT
 *
 * Any out-of-sequence sensor fires a DOPPEL_FEHLER (fault).
 * A 4-second timeout on any non-terminal state also fires a fault.
 * The fault state is held for 600 ms so the 50 Hz telemetry always catches it.
 */
class BallWechselCounter {

private:
    enum States : uint8_t {
        /* 00 */ IDLE,
        /* 01 */ WAITING_SERVE,

        /* Left serve path */
        /* 02 */ LINKS_AUFSCHLAG_LEFT_TABLE,
        /* 03 */ LINKS_AUFSCHLAG_RIGHT_TABLE,
        /* 04 */ LINKS_BALLWECSHEL_RIGHT_RACKET,
        /* 05 */ LINKS_BALLWECHSEL_LEFT_TABLE,
        /* 06 */ LINKS_BALLWECHSEL_LEFT_RACKET,
        /* 07 */ LINKS_BALLWECHSEL_RIGHT_TABLE,
        /* 08 */ LINKS_BALLWECHSEL_ERFOLGREICH_RIGHT_RACKET,
        /* 09 */ LINKS_BALLWECHSEL_COUNT,

        /* Right serve path */
        /* 10 */ RECHTS_AUFSCHLAG_RIGHT_TABLE,
        /* 11 */ RECHTS_AUFSCHLAG_LEFT_TABLE,
        /* 12 */ RECHTS_BALLWECHSEL_LEFT_RACKET,
        /* 13 */ RECHTS_BALLWECHSEL_RIGHT_TABLE,
        /* 14 */ RECHTS_BALLWECSHEL_RIGHT_RACKET,
        /* 15 */ RECHTS_BALLWECHSEL_LEFT_TABLE,
        /* 16 */ RECHTS_BALLWECHSEL_ERFOLGREICH_LEFT_RACKET,
        /* 17 */ RECHTS_BALLWECHSEL_COUNT,

        /* 18 */ DOPPEL_FEHLER,
        /* 19 */ FINISH_GAME
    };

public:
    BallWechselCounter(Racket &leftRacket, Racket &rightRacket,
                       Table  &leftTable,  Table  &rightTable)
        : leftRacket_(leftRacket), rightRacket_(rightRacket),
          leftTable_(leftTable),   rightTable_(rightTable) {}

    // ── Telemetry getters ──────────────────────────────────────────────────────

    /**
     * Phase for GameTelemetry.phase:
     *   0 = Sensor test / IDLE
     *   1 = Waiting for serve
     *   2 = Rally in progress
     *   3 = Fault (DOPPEL_FEHLER)
     *   4 = Finished
     */
    uint8_t getPhase() const {
        switch (state_) {
            case IDLE:           return 0;
            case WAITING_SERVE:  return 1;
            case DOPPEL_FEHLER:  return 3;
            case FINISH_GAME:    return 4;
            default:             return 2;
        }
    }

    /**
     * Expected next sensor for GameTelemetry.expectedInput:
     *   0 = none/any,  1 = Left racket,  2 = Left table,
     *   3 = Right table,  4 = Right racket
     */
    uint8_t getExpectedInput() const {
        switch (state_) {
            case LINKS_AUFSCHLAG_LEFT_TABLE:
            case LINKS_BALLWECHSEL_LEFT_TABLE:
            case RECHTS_AUFSCHLAG_LEFT_TABLE:
            case RECHTS_BALLWECHSEL_LEFT_TABLE:                return 2; // left table
            case LINKS_AUFSCHLAG_RIGHT_TABLE:
            case LINKS_BALLWECHSEL_RIGHT_TABLE:
            case RECHTS_AUFSCHLAG_RIGHT_TABLE:
            case RECHTS_BALLWECHSEL_RIGHT_TABLE:               return 3; // right table
            case LINKS_BALLWECSHEL_RIGHT_RACKET:
            case LINKS_BALLWECHSEL_ERFOLGREICH_RIGHT_RACKET:
            case RECHTS_BALLWECSHEL_RIGHT_RACKET:              return 4; // right racket
            case RECHTS_BALLWECHSEL_LEFT_RACKET:
            case RECHTS_BALLWECHSEL_ERFOLGREICH_LEFT_RACKET:
            case LINKS_BALLWECHSEL_LEFT_RACKET:                return 1; // left racket
            default:                                           return 0;
        }
    }

    uint16_t getRallyCount() const { return static_cast<uint16_t>(totalBallWechsel_); }
    bool     isFault()       const { return state_ == DOPPEL_FEHLER; }

    // Legacy accessor kept for existing callers
    byte getTotalBallwechsel() { return static_cast<byte>(totalBallWechsel_); }

    // ── Main loop ─────────────────────────────────────────────────────────────

    void loop() {
        // Reset timeout timer whenever the state changes
        if (state_ != prevState_) {
            timeoutMs_ = 0;
            if (state_ == DOPPEL_FEHLER) faultMs_ = 0;
            prevState_ = state_;
        }

        // 4-second timeout on any active (non-terminal) state → fault
        static constexpr uint32_t TIMEOUT_MS = 4000;
        const bool isActive = (state_ != IDLE && state_ != WAITING_SERVE &&
                               state_ != DOPPEL_FEHLER && state_ != FINISH_GAME &&
                               state_ != LINKS_BALLWECHSEL_COUNT &&
                               state_ != RECHTS_BALLWECHSEL_COUNT);
        if (isActive && timeoutMs_ > TIMEOUT_MS) {
            state_ = DOPPEL_FEHLER;
            return;
        }

        switch (state_) {

            case IDLE:
                state_ = WAITING_SERVE;
                break;

            case WAITING_SERVE:
                if (leftRacket_.isHit())       state_ = LINKS_AUFSCHLAG_LEFT_TABLE;
                else if (rightRacket_.isHit()) state_ = RECHTS_AUFSCHLAG_RIGHT_TABLE;
                break;

            // ── Left serve path ──────────────────────────────────────────────

            case LINKS_AUFSCHLAG_LEFT_TABLE:
                if (leftTable_doppelFehler()) { state_ = DOPPEL_FEHLER; break; }
                if (leftTable_.isHit())         state_ = LINKS_AUFSCHLAG_RIGHT_TABLE;
                break;

            case LINKS_AUFSCHLAG_RIGHT_TABLE:
                if (rightTable_doppelFehler()) { state_ = DOPPEL_FEHLER; break; }
                if (rightTable_.isHit())         state_ = LINKS_BALLWECSHEL_RIGHT_RACKET;
                break;

            case LINKS_BALLWECSHEL_RIGHT_RACKET:
                if (rightRacket_doppelFehler()) { state_ = DOPPEL_FEHLER; break; }
                if (rightRacket_.isHit())         state_ = LINKS_BALLWECHSEL_LEFT_TABLE;
                break;

            case LINKS_BALLWECHSEL_LEFT_TABLE:
                if (leftTable_doppelFehler()) { state_ = DOPPEL_FEHLER; break; }
                if (leftTable_.isHit())         state_ = LINKS_BALLWECHSEL_LEFT_RACKET;
                break;

            case LINKS_BALLWECHSEL_LEFT_RACKET:
                if (leftRacket_doppelFehler()) { state_ = DOPPEL_FEHLER; break; }
                if (leftRacket_.isHit())         state_ = LINKS_BALLWECHSEL_RIGHT_TABLE;
                break;

            case LINKS_BALLWECHSEL_RIGHT_TABLE:
                if (rightTable_doppelFehler()) { state_ = DOPPEL_FEHLER; break; }
                if (rightTable_.isHit())         state_ = LINKS_BALLWECHSEL_ERFOLGREICH_RIGHT_RACKET;
                break;

            case LINKS_BALLWECHSEL_ERFOLGREICH_RIGHT_RACKET:
                if (rightRacket_doppelFehler()) { state_ = DOPPEL_FEHLER; break; }
                if (rightRacket_.isHit())         state_ = LINKS_BALLWECHSEL_COUNT;
                break;

            case LINKS_BALLWECHSEL_COUNT:
                ++totalBallWechsel_;
                state_ = LINKS_BALLWECHSEL_LEFT_TABLE;   // continue rally from left table
                break;

            // ── Right serve path ─────────────────────────────────────────────

            case RECHTS_AUFSCHLAG_RIGHT_TABLE:
                if (rightTable_doppelFehler()) { state_ = DOPPEL_FEHLER; break; }
                if (rightTable_.isHit())         state_ = RECHTS_AUFSCHLAG_LEFT_TABLE;
                break;

            case RECHTS_AUFSCHLAG_LEFT_TABLE:
                if (leftTable_doppelFehler()) { state_ = DOPPEL_FEHLER; break; }
                if (leftTable_.isHit())         state_ = RECHTS_BALLWECHSEL_LEFT_RACKET;
                break;

            case RECHTS_BALLWECHSEL_LEFT_RACKET:
                if (leftRacket_doppelFehler()) { state_ = DOPPEL_FEHLER; break; }
                if (leftRacket_.isHit())         state_ = RECHTS_BALLWECHSEL_RIGHT_TABLE;
                break;

            case RECHTS_BALLWECHSEL_RIGHT_TABLE:
                if (rightTable_doppelFehler()) { state_ = DOPPEL_FEHLER; break; }
                if (rightTable_.isHit())         state_ = RECHTS_BALLWECSHEL_RIGHT_RACKET;
                break;

            case RECHTS_BALLWECSHEL_RIGHT_RACKET:
                if (rightRacket_doppelFehler()) { state_ = DOPPEL_FEHLER; break; }
                if (rightRacket_.isHit())         state_ = RECHTS_BALLWECHSEL_LEFT_TABLE;
                break;

            case RECHTS_BALLWECHSEL_LEFT_TABLE:
                if (leftTable_doppelFehler()) { state_ = DOPPEL_FEHLER; break; }
                if (leftTable_.isHit())         state_ = RECHTS_BALLWECHSEL_ERFOLGREICH_LEFT_RACKET;
                break;

            case RECHTS_BALLWECHSEL_ERFOLGREICH_LEFT_RACKET:
                if (leftRacket_doppelFehler()) { state_ = DOPPEL_FEHLER; break; }
                if (leftRacket_.isHit())         state_ = RECHTS_BALLWECHSEL_COUNT;
                break;

            case RECHTS_BALLWECHSEL_COUNT:
                ++totalBallWechsel_;
                state_ = RECHTS_BALLWECHSEL_LEFT_TABLE;  // continue rally from left table
                break;

            // ── Terminal states ───────────────────────────────────────────────

            case DOPPEL_FEHLER:
                // Hold fault visible for 600 ms — guarantees at least one 50 Hz
                // telemetry frame sees phase = 3 before we reset to IDLE
                if (faultMs_ >= 600) state_ = IDLE;
                break;

            case FINISH_GAME:
                break; // stay until explicitly reset

            default:
                state_ = IDLE;
                break;
        }
    }

    // ── Debug ─────────────────────────────────────────────────────────────────

    void printDebug() {
        if (lastPrintState_ != state_) {
            Serial.print(F("BWC state="));   Serial.print(static_cast<int>(state_));
            Serial.print(F(" phase="));      Serial.print(getPhase());
            Serial.print(F(" expected="));   Serial.print(getExpectedInput());
            Serial.print(F(" rallies="));    Serial.println(totalBallWechsel_);
            lastPrintState_ = state_;
        }
    }

private:
    // ── Fault predicates ───────────────────────────────────────────────────────
    // Each returns true if a sensor fires that shouldn't in the current wait.

    bool leftRacket_doppelFehler() const {
        // Waiting for left racket → fault if anything else fires
        // BUG-FIX: original code checked leftTable_ twice, never rightTable_
        return leftTable_.isHit() || rightRacket_.isHit() || rightTable_.isHit();
    }

    bool rightRacket_doppelFehler() const {
        return leftRacket_.isHit() || rightTable_.isHit() || leftTable_.isHit();
    }

    bool rightTable_doppelFehler() const {
        return leftRacket_.isHit() || rightRacket_.isHit() || leftTable_.isHit();
    }

    bool leftTable_doppelFehler() const {
        return leftRacket_.isHit() || rightRacket_.isHit() || rightTable_.isHit();
    }

private:
    Racket &leftRacket_;
    Racket &rightRacket_;
    Table  &leftTable_;
    Table  &rightTable_;

    int    totalBallWechsel_{0};
    States state_{IDLE};
    States prevState_{IDLE};
    States lastPrintState_{IDLE};

    elapsedMillis timeoutMs_{};
    elapsedMillis faultMs_{};
};
