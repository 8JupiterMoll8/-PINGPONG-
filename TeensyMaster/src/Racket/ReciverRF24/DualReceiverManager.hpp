#ifndef DUAL_RECEIVER_MANAGER_H
#define DUAL_RECEIVER_MANAGER_H
#pragma once

#include <Arduino.h>
#include "Reciver.hpp"

/**
 * DualReceiverManager
 * 
 * Manages two NRF24L01+ radios sharing the same SPI bus.
 * Both radios listen continuously. The RF24 library selects the correct radio
 * for each short SPI transaction using that radio's dedicated CSN pin.
 * 
 * The manager makes sure both radios are deselected before initialization,
 * polls both on every loop, and provides packet statistics for debugging.
 */
class DualReceiverManager
{
private:
    Reciver &left_;
    Reciver &right_;

    const byte lr_csn_pin_;
    const byte rr_csn_pin_;

public:
    DualReceiverManager(
        Reciver &left, Reciver &right,
        byte lr_csn, byte rr_csn)
        : left_(left), right_(right),
          lr_csn_pin_(lr_csn), rr_csn_pin_(rr_csn)
    {
    }

    /**
     * Deselect both SPI devices before either RF24 instance starts using the
     * shared bus, then initialize both receivers. RF24::begin() provides the
     * radio's required power-up delays and manages CSN for every command.
     */
    bool setup()
    {
        pinMode(lr_csn_pin_, OUTPUT);
        pinMode(rr_csn_pin_, OUTPUT);

        digitalWrite(lr_csn_pin_, HIGH);
        digitalWrite(rr_csn_pin_, HIGH);

        const bool rightReady = right_.setup();
        const bool leftReady = left_.setup();
        const bool bothReady = leftReady && rightReady;

        if (bothReady)
        {
            Serial.println(F("[DualReceiverManager] Both radios are listening."));
        }
        else
        {
            Serial.println(F("[DualReceiverManager] WARNING: one or both radios failed."));
        }

        return bothReady;
    }

    /**
     * Poll both continuously. Each call drains that radio's FIFO so the shared
     * ReciverData object contains its newest complete sample.
     */
    void loop()
    {
        left_.readLatest();
        right_.readLatest();
    }

    // --- Debug & Statistics ---

    uint32_t getLeftPackets()  const { return left_.getPacketsReceived();  }
    uint32_t getRightPackets() const { return right_.getPacketsReceived(); }
    uint32_t getLeftPacketAgeMs() const { return left_.getPacketAgeMs(); }
    uint32_t getRightPacketAgeMs() const { return right_.getPacketAgeMs(); }
    bool isLeftFresh(uint32_t timeoutMs) const { return left_.hasFreshPacket(timeoutMs); }
    bool isRightFresh(uint32_t timeoutMs) const { return right_.hasFreshPacket(timeoutMs); }

    /**
     * Print packet reception statistics to Serial.
     * Call this periodically (e.g., every second) for debugging.
     */
    void printStats()
    {
        Serial.print(F("[RF24] L_pkts="));
        Serial.print(getLeftPackets());
        Serial.print(F("  R_pkts="));
        Serial.println(getRightPackets());
    }

    /**
     * Reset packet counters. Useful for per-second rate measurement.
     */
    void resetStats()
    {
        left_.resetStats();
        right_.resetStats();
    }
};

#endif /* DUAL_RECEIVER_MANAGER_H */
