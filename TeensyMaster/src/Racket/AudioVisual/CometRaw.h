#ifndef COMETRAW_H
#define COMETRAW_H

#include <FastLED.h>
#include "CometRaw.h"
#pragma once

class CometRaw
{

public:
  CometRaw(CRGB (&ledStrip)[58]) : _leds(ledStrip)
  {
  }

  enum States
  {
    WAITING,
    MOVING,
    COLLIDING,
    EXPLOSION
  } state;

public:
  void loop()
  {
    
    if (ms > 10)
    {
      ms = 0;
          stateMachine();
            //movingWithoutCollision();
          // movingInLoop();
    }
  }

/**
 * Comet Behaviours
 * 
 */
public:
  void setStartPosition(float position ) { _iPos = position;   _resetStartPosition = position ;}
  void resetStartPosition()              { _iPos = _resetStartPosition; }
  void setSpeed (float speed    )        { _speed        = speed;    }
  void setFadeSize (int fadeSize)        { _fadeAmt      = fadeSize; }
  void setWidth(int size)                { _size         = size;     }
  int  width ()                          { return _size;}
  void reverseDirection()                { _iDirection *= -1;        }
  void setMidiVelocity (int v )          { _midiVelocity = v;        }
  void start()                           { _animationStart = true;   }
  void stop()                            { _animationStart = false;  }
  auto xPos()                            { return _iPos;             }
 


/**
 * Set the Color object
 * 
 */
  void setColor(byte red, byte green, byte blue )
  {
    _red   = red;
    _blue  = blue;
    _green = green;
  }
  

/**
 * @brief Accelarte and Move   Comet
 * 
 */
  void move()
  {
     _speed = _speed + _acceleration;              // Accelerate Comet
     _iPos += _iDirection * _speed;                // Velocity
  }

    void moveWithoutAcceleration()
  {
     
     _iPos += _iDirection * _speed;                // Velocity
  }


private:
  void updatePosition()
  {
    for (int i = 0; i < _size; i++)
    {
      _leds[(int)_iPos + i] = CRGB(_red, _green, _blue); 
      usbMIDI.sendNoteOn(map(_iPos, 0, 58, 30, 127), _midiVelocity, 2);
    }
  }

/**
 * Fade the End of the Comet
 * This makes a COmet diferent from a constant line
 * 
 */
private:
  void randomFadeTail()
  {
    for (int j = 0; j < _numLeds; j++)
    {
      if (random(10) > 5)
      {
        _leds[j] = _leds[j].fadeToBlackBy(_fadeAmt);
      }
      usbMIDI.sendNoteOff(map(j, 0, 58, 30, 127), 75, 2);
    }
  }

/**
 * Check if Comet Hit the end of the Boundaries'
 * ! Important _iPos does never be -1.0 or 361.0
 * 
 */
private:
  boolean isReachBoundary()
  {
    if (int(_iPos) >= 58  || int(_iPos) <= 0 )
      return true;
    else
      return false;
  }

void stateMachine()
{
    switch (state)
      {
      case WAITING:
        if (_animationStart == true) state = MOVING;
        break;
      case MOVING:
      // Do: 
          move();
      // Leave
        if (isReachBoundary())
        {
          resetStartPosition();
          stop();
          FastLED.clear();
         
         state= WAITING;
        }
          updatePosition();
          randomFadeTail();
          FastLED.show();
      
        break;

      default:
        break;
      }

}

/**
 * @brief This the simple form of moving Comet
 * 
 */
void  movingWithoutCollision()
{
   if (_animationStart == true) move();

      if (isReachBoundary()) { 
        resetStartPosition ();  
        stop();

      }else{ 

        updatePosition(); 
        randomFadeTail();
      }

}

void  movingInLoop()
{      
        moveWithoutAcceleration();

      if (isReachBoundary()) { resetStartPosition (); } 

        updatePosition(); 
        randomFadeTail();

}

  

private:
  CRGB (&_leds)[58];
  int      _numLeds            = 58;
  float   _startPosition       = 0.0;
  float   _resetStartPosition  = 58.0;
  byte    _fadeAmt             = 200;
  int     _size                = 7;
  float   _speed               = 1.25;
  float   _acceleration        = 0.001;
  float   _iPos                = 0.0;
  float   _iDirection          = 1.0;
  byte    _midiVelocity        = 75;
  boolean _animationStart      = false;
  byte    _red                 = 255;
  byte    _green               = 255;
  byte    _blue                = 255;     

  elapsedMillis ms;
 

};

#endif