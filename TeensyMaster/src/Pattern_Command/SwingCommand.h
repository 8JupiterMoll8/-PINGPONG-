#ifndef SWINGCOMMAND_H
#define SWINGCOMMAND_H

#pragma once
#include "PingPong.h"
#include "elapsedMillis.h"

class SwingCommand : public Command {
 public:
  SwingCommand(Racket &racket, byte midiChannel)
      : m_racket(racket), m_midiChannel(midiChannel) {}

 private:
  Racket &m_racket;
  byte m_midiChannel = 3;
  elapsedMillis msec;

  // the MIDI continuous controller for each analog input
  const int controllerA0 = 14;  // 10 = volume A
  const int controllerA1 = 15;  // 11 = volume B

  // store previously sent values, to detect changes
  int previousA0 = -1;
  int previousA1 = -1;

 public:
  virtual void execute() override { RacketSwingToMidiCC(); }

 private:
  void RacketSwingToMidiCC() {


    // only check the analog inputs 50 times per second,
    // to prevent a flood of MIDI messages
    if (msec >= 5) {
      msec = 0;
      int n0 =  map(m_racket.swing_A_Gain(),0.0,0.5,0,127);
      int n1 = map(m_racket.swing_B_Gain(),0.0,0.5,0,127);

      // only transmit MIDI messages if analog input changed
      if (n0 != previousA0) {
        usbMIDI.sendControlChange(controllerA0, n0, m_midiChannel);
        previousA0 = n0;
     // Serial.println(n0);

      }

      if (n1 != previousA1) {
        usbMIDI.sendControlChange(controllerA1, n1, m_midiChannel);
        previousA1 = n1;
        // Serial.println(n1);
      }
    }
  }
  
};

#endif