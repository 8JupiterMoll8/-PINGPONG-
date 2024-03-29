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

public:
	Reciver(RF24 &RADIO, const uint64_t ADRESS, const byte CHANNEL, ReciverData &RECIVER_DATA) : radio(RADIO),
																								adress(ADRESS),
																								channel(CHANNEL),
																								reciverData(RECIVER_DATA)
	{
	}

	void setup()
	{
		if (!radio.begin())
		{
			Serial.println(F("radio hardware is not responding!!"));
			while (1)
			{
			}
		}
		else
		{
			Serial.println(F("Reciver Teensy LC - 121 LEFT Racket!!"));
		}

		radio.setChannel(channel);
		radio.setPALevel(RF24_PA_MAX);
		radio.setPayloadSize(sizeof(reciverData));
		radio.setAutoAck(false);
		radio.setDataRate(RF24_2MBPS);
		radio.openReadingPipe(0, adress);
		radio.startListening();

		// For debugging info
		printf_begin();				// needed only once for printing details
		radio.printDetails();		// (smaller) function that prints raw register values
		radio.printPrettyDetails(); // (larger) function that prints human readable data
	}

	void loop()
	{
		//static elapsedMillis nCurrentTime;
		uint8_t pipe;

		if (radio.available(&pipe))
		{
			//Serial.println(reciverData.fsr);

			uint8_t bytes = radio.getPayloadSize();
			radio.read(&reciverData, bytes);
		}
	}

	void isAvaiable() 
	{
		
	}
	};
#endif