#ifndef LEFTCLOCK_H
#define LEFTCLOCK_H

#pragma once
#include "AccelStepper.h"
#include "Clock.h"
#include "MoveConstant.h"
#include "MoveRandomly.h"
#include "MoveTickTack.h"
#include "elapsedMillis.h"

class LeftClock : public Clock {
 private:

 elapsedMillis ms;
 boolean toogle = false;

 public:


  LeftClock(AccelStepper stepper): Clock(stepper)
  {
    moveConstant   = new MoveConstant(m_stepper);
    moveRandom     = new MoveRandomly(m_stepper);
    moveTickTack   = new MoveTickTack(m_stepper);

    moveBehaviour =  moveRandom;

  }

  void setup(){
    setupMoveBehaviour();
    // m_stepper.setMaxSpeed(12800);
    // m_stepper.setSpeed(10000); // had to slow for my motor
    // m_stepper.setAcceleration(100.0);
  
  }

  void loop()
  {

    if(ms >5000)
    {
      ms=0;
      toogle = !toogle;

    }
    
   
    
    if(toogle ==false) {
      setSpeedMoveBehavoiur(500);
      setMoveBehaviour(moveConstant);
      }

    if(toogle == true) {
      setSpeedMoveBehavoiur(1000);
      setMoveBehaviour(moveConstant);
    }

      executeMoveBehaviour();


    

  }
 

};

#endif