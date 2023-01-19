#ifndef BARGRAPH_H
#define BARGRAPH_H

#pragma once
#include <FastLED.h>
#include "Racket.hpp"
class Bargraph
{
private:
  CRGB(&m_A_ledStrip)[58];
  const float ledCount = 58.0;
  float _speed = 0;
  float m_speed_max = 0.0;
  float m_speed_min = 30.0;

public:
  Bargraph(CRGB (&ledStrip)[58]) : m_A_ledStrip(ledStrip)
  {
  }

public:
  void loop()
  {
    // map the result to a range from 0 to the number of LEDs:
    int ledLevel = map(_speed, m_speed_min, m_speed_max, 0.0, ledCount);

    // Serial.println(ledLevel);
    // delay(10);

    // loop over the LED array:
    for (int thisLed = 0; thisLed < ledCount; thisLed++)
    {
      // if the array element's index is less than ledLevel,
      // turn the pin for this element on:
      if (thisLed < ledLevel)
      {
        m_A_ledStrip[thisLed] += CRGB(255, 255, 255);
        usbMIDI.sendNoteOn(thisLed, 75, 3);

        for (int j = 0; j < ledLevel; j++)
        {

         m_A_ledStrip[j] += m_A_ledStrip[j].fadeToBlackBy(180 - j);
        }
      }
      // turn off all pins higher than the ledLevel:
      else
      {
        usbMIDI.sendNoteOff(thisLed, 0, 3);
        m_A_ledStrip[thisLed] = m_A_ledStrip[thisLed].fadeToBlackBy(34);
      }
    }
  }

  void setMapSpeed(float input, float min, float max)
  {
    _speed     = input;
    m_speed_min = min;
    m_speed_max = max;
  }
};

#endif