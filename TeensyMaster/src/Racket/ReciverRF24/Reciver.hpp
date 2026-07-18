#ifndef RECIVER_H
#define RECIVER_H
#pragma once
#include "RF24.h"
#include "ReciverData.hpp"


class Reciver
{
private:
	RF24           &radio;
	const uint64_t adress;
	const byte     channel;
	ReciverData    &reciverData;

	// Packet statistics for debugging reception quality.
	uint32_t packetsReceived_ = 0;
	uint32_t lastPacketMs_ = 0;
	bool hasReceivedPacket_ = false;

public:
	Reciver(RF24 &RADIO, const uint64_t ADRESS, const byte CHANNEL, ReciverData &RECIVER_DATA) : radio(RADIO),
																								adress(ADRESS),
																								channel(CHANNEL),
																								reciverData(RECIVER_DATA)
	{
	}

	bool setup()
	{
		if (!radio.begin())
		{
			if (channel == 121) Serial.println(F(" LEFT Reciver -121 is not responding!!"));
			if (channel == 125) Serial.println(F(" RIGHT Reciver-125 is not responding!!"));
			return false;
		}

		if (channel == 121) Serial.println(F(" LEFT Reciver  -121 running !!"));
		if (channel == 125) Serial.println(F(" RIGHT Reciver -125 running !!"));

		radio.setChannel(channel);
		radio.setPALevel(RF24_PA_MAX);
		radio.setPayloadSize(sizeof(reciverData));
		radio.setAutoAck(false);
		radio.setDataRate(RF24_2MBPS);
		radio.setCRCLength(RF24_CRC_16);
		radio.openReadingPipe(0, adress);
		radio.startListening();

		// For debugging info
		//printf_begin();				// needed only once for printing details
		radio.printDetails();		// (smaller) function that prints raw register values
		//radio.printPrettyDetails(); // (larger) function that prints human readable data

		return true;
	}

	/**
	 * Original loop() kept for backward compatibility. It now drains all queued
	 * packets so reciverData contains the newest complete sensor sample.
	 */
	void loop()
	{
		readLatest();
	}

	/**
	 * Read every complete packet currently queued by the radio.
	 * The NRF24 FIFO is oldest-first, so draining it leaves reciverData holding
	 * the freshest available racket values.
	 */
	uint8_t readLatest()
	{
		uint8_t pipe;
		uint8_t packetsRead = 0;

		while (radio.available(&pipe))
		{
			const uint8_t bytes = radio.getPayloadSize();
			radio.read(&reciverData, bytes);
			packetsReceived_++;
			packetsRead++;
		}

		if (packetsRead > 0)
		{
			lastPacketMs_ = millis();
			hasReceivedPacket_ = true;
		}

		return packetsRead;
	}

	/**
	 * Compatibility helper for code that only needs to know whether new data
	 * arrived. All queued packets are still drained to keep the latest sample.
	 */
	bool readIfAvailable()
	{
		return readLatest() > 0;
	}

	// --- Statistics for debugging ---

	uint32_t getPacketsReceived() const { return packetsReceived_; }
	bool hasReceivedPacket() const { return hasReceivedPacket_; }

	uint32_t getPacketAgeMs() const
	{
		return hasReceivedPacket_ ? millis() - lastPacketMs_ : UINT32_MAX;
	}

	bool hasFreshPacket(uint32_t timeoutMs) const
	{
		return hasReceivedPacket_ && getPacketAgeMs() < timeoutMs;
	}

	void resetStats()
	{
		packetsReceived_ = 0;
	}
};
#endif
