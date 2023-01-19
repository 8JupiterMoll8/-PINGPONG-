#ifndef ROLLPITCHYAWCOMMAND_H
#define ROLLPITCHYAWCOMMAND_H

#pragma once
#include "PingPong.h"
#include "elapsedMillis.h"

class RollPitchYawCommand : public Command
{
public:
    RollPitchYawCommand(Racket &racket, byte midiChannel)
      : m_racket(racket), m_midiChannel(midiChannel) {}

 private:
  Racket &m_racket;
  byte m_midiChannel = 3;
  elapsedMillis msec;

    // the MIDI continuous controller for each analog input
  const int controllerA0 = 14;  // 14 = roll A
  const int controllerA1 = 15;  // 16 = pitch B
  const int controllerA2 = 16;  // 17 = yaw C

  // store previously sent values, to detect changes
  int previousA0 = -1;
  int previousA1 = -1;
  int previousA2 = -1;

public:
 public:
  virtual void execute() override { RollPitchYawToMidiCC();}
private:

void RollPitchYawToMidiCC() {


    // only check the analog inputs 50 times per second,
    // to prevent a flood of MIDI messages
    if (msec >= 25) {
      msec = 0;
      int n0 =  map(m_racket.roll(),-180.0, 180.0, 0, 127);
      int n1 =  map(m_racket.pitch(),-180.0, 180.0, 0, 127);;
      int n2 =  map(m_racket.yaw(), 0.0, 360.0, 0, 127);

      // only transmit MIDI messages if analog input changed
      if (n0 != previousA0) {
        usbMIDI.sendControlChange(controllerA0, n0, m_midiChannel);
        previousA0 = n0;

        Serial.print("Roll: ");
        Serial.print(n0);

      }

      if (n1 != previousA1) {
        usbMIDI.sendControlChange(controllerA1, n1, m_midiChannel);
        previousA1 = n1;
        Serial.print("Pitch: ");
        Serial.print(n1);
      }

       if (n2 != previousA2) {
        usbMIDI.sendControlChange(controllerA1, n2, m_midiChannel);
        previousA2 = n2;
        Serial.print("Yaw: ");
        Serial.println(n1);
      }
    }
  }

};

#endif