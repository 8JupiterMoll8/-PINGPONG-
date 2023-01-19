#ifndef LEFTCLOCK_H
#define LEFTCLOCK_H

#pragma once
#include "AccelStepper.h"
#include "Clock.h"
#include "MoveConstant.h"
#include "elapsedMillis.h"

class LeftClock : public Clock {
 private:

 elapsedMillis ms;
 boolean toogleMoveBehaviour = false;
 public:


  LeftClock(int pinStep, int pinDir): Clock(pinStep,pinDir)
  {
   
    moveBehaviour = new MoveConstant(stepper);
  }


};

#endif