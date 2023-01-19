#ifndef AUDIOVISUALIZER_H
#define AUDIOVISUALIZER_H

#pragma once
#include "Racket.hpp"

class AudioVisualizer {
 public:
  AudioVisualizer(Racket &racket) : m_Racket(racket){};
  void loop(){

    midiHit();
  };

 private:
  Racket &m_Racket;

  void midiHit() 
  {
    static int counter = 0;
    if (m_Racket.isHit()) 
    {
        Serial.println(m_Racket.hitPeak());
        byte velocity = m_Racket.hitPeak(); 
      usbMIDI.sendNoteOn(54, velocity, 1);
      //Serial.println("Hit");
     // Serial.println(counter++);
     //delay(10);


    } else 
    {
      //usbMIDI.sendNoteOff(127, 0, 1);
     //  delay(1);
    }
  }
};
#endif