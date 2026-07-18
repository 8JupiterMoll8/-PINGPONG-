#ifndef COMET_H
#define COMET_H

#include <FastLED.h>
#pragma once
extern const int NUM_LEDS;

class Comet
{

public:
  // Constructor — forward-moving comet (start at position 0)
  Comet(CRGB (&ledStrip)[58]):
  _leds(ledStrip)
  {
  }

  // Constructor — reverse-moving comet (start at the far end)
  Comet(CRGB (&ledStrip)[58], int /*start*/):
  _leds(ledStrip)
  {
      _iPos          = NUM_LEDS - 1;
      _startPosition = NUM_LEDS - 1;
      _iDirection   *= -1;
  }

  void loop()
  {
    switch (state)
    {
    case WAIT:
      if (_animationCometStart == true) {
        state = START;
      }
      break;

    case START:
      animationComet();
      _animationCometStart = false;
      if (animationCometIsEnd() == true) {
        state = END;
      }
      break;

    case END:
      _animationCometStart = false;
      state = WAIT;
      _iPos = _startPosition;
      break;

    default:
      break;
    }
  }

  // Behaviours
  void start()                   { _animationCometStart = true; _iPos = _startPosition; }
  void reverseDirection()        { _iDirection *= -1; }
  void setSpeed(float speed)     { _speed       = speed; }
  void setFadeSize(int fadeSize) { _fadeAmt     = fadeSize; }
  void setWidth(int size)        { _size        = size; }
  void setMidiVelocity(int v)    { _midiVelocity = v; }

  void animationComet()
  {
    if (ms > 10)
    {
      ms = 0;
      _speed += _acceleration;
      _iPos  += _iDirection * _speed;

      // Draw comet head — clamp so we never write past the array end
      for (int i = 0; i < _size; i++)
      {
        int idx = constrain((int)_iPos + i, 0, NUM_LEDS - 1);
        _leds[idx] += CRGB(255, 255, 255);
        int n = map((int)_iPos, 0, NUM_LEDS, 30, 127);
        usbMIDI.sendNoteOn(n, _midiVelocity, 2);
      }

      // Fade tail across the whole strip
      for (int j = 0; j < NUM_LEDS; j++)
      {
        if (random(10) > 5)
        {
          _leds[j] += _leds[j].fadeToBlackBy(_fadeAmt);
          usbMIDI.sendNoteOff(j, 75, 2);
        }
      }
    }
  }

  boolean animationCometIsEnd()
  {
    if (_iPos > (NUM_LEDS - _size) || _iPos < 0.0F)
    {
      Serial.print("Comet hit boundary at ");
      Serial.println(_iPos);
      FastLED.clear();
      return true;
    }
    return false;
  }

private:
  CRGB (&_leds)[58];   // sized to match main.cpp — change together with NUM_LEDS

  byte    _fadeAmt             = 124;
  int     _size                = 5;
  float   _speed               = 1.0F;
  float   _acceleration        = 0.01F;
  float   _iPos                = 0.0F;
  float   _startPosition       = 0.0F;
  float   _iDirection          = 1.0F;
  byte    _midiVelocity        = 75;
  boolean _animationCometStart = false;

  elapsedMillis ms;
  enum States { WAIT, START, END };
  States state = WAIT;
};

#endif