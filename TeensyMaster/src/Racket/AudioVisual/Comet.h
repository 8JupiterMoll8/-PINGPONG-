#ifndef COMET_H
#define COMET_H

#include <FastLED.h>
#pragma once
extern const int NUM_LEDS;
class Comet
{

public:
  Comet(CRGB (&ledStrip)[360]): 
  _leds(ledStrip)
  {
      //_iPos = 0.00;
     // _startPosition = 0;
  } 

  Comet(CRGB (&ledStrip)[360], int start): 
  _leds(ledStrip)
  {
     // _iPos = NUM_LEDS - _size;
      _iPos          = 360 - 1;
      _startPosition = 360 - 1 ;
      _iDirection    *= -1;
  
  } 

  void loop()
  {
    switch (state)
    {
    case WAIT:
      //Leave State:
      if(_animationCometStart == true) {
        state = START;
      } 
      break;

    case START:
      //Do:
      animationComet();
       _animationCometStart = false;
    
      //Leave State
      if(animationCometIsEnd()== true)
      {
       state = END; 
    
      }
      break;

    case END:
      //Do:
       _animationCometStart = false;
      //Leave State
      state = WAIT;
      _iPos = _startPosition;

      break;

    default:
      break;
    }
  }
  // Behaviours
  void start()                   { _animationCometStart = true; _iPos = _startPosition;  }
  void reverseDirection()        { _iDirection *= -1;           }
  void setSpeed(float speed)     { _speed       = speed;        }
  void setFadeSize(int fadeSize) { _fadeAmt     = fadeSize;     }
  void setWidth(int size)         { _size        = size;         }
  void setMidiVelocity(int v)    { _midiVelocity = v;           }
  

  void animationComet()
  {
    if (ms > 10)
    {
      ms = 0;
      _speed  = _speed +  _acceleration;
      _iPos  += _iDirection * _speed;
     // Serial.println(_size);
     

     
      for (int i = 0; i < _size; i++)
      {
        _leds[(int)_iPos + i] += CRGB(255, 255, 255); // Move Forward
        int  n = map(_iPos,0,360,0,127);              
        usbMIDI.sendNoteOn(n, _midiVelocity, 2);      //Make Sound
      }

      // Randomly fade the LEDs
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
      Serial.println(_size);
   // int(_iPos) = _iPos;
    // Check if Comet hits end and reverse Direction
    //!Something with _size is wrong i have to take look
    //! Right now i change  (NUM_LEDS - _size) to (NUM_LEDS - 1)
    if (_iPos > (NUM_LEDS - _size ) || _iPos < (float) 0.0)
    {
     // _iPos = 0;
      _iPos = 360 - 1;
      //reverseDirection();
      Serial.print("HIT BORDER at ");
      Serial.print(_iPos);
     // _iPos = _startPosition;
      Serial.print("HIT BORDER at ");
      Serial.print(_iPos);
      FastLED.clear();

      return true;
    }
    else
    {
      return false;
    }
  }


private:
  CRGB (&_leds)[360];
  const int NUM_LEDS = 360;
 

  byte    _fadeAmt             = 124;
  int     _size                = 5;
  float   _speed               = 1.0;
  float   _acceleration        = 0.01;
  float   _iPos                = 0.0;
  float   _startPosition       = 0.0;
  float   _iDirection          = 1.0;
  byte    _midiVelocity        = 75;
  boolean _animationCometStart = false;

  elapsedMillis ms;
  enum States { WAIT, START, END};
  States state = WAIT;
};

#endif