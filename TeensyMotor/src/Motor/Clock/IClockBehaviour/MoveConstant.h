#ifndef CLOCK_CONSTANT_MOVE_H
#define CLOCK_CONSTANT_MOVE_H

#pragma once

#include "IMoveBehaviour.h"

class MoveConstant : public IMoveBehaviour
{

public:
  MoveConstant(AccelStepper &accelStepper) :  IMoveBehaviour(accelStepper)
  {
      m_speed = 4000;
  }

  virtual void move() override
  {
    StepperMotor.setSpeed(m_speed);
    StepperMotor.runSpeed();
    //Zeiger.runNow();
    Serial.println("MoveConstant");
  
  }

  virtual void setSpeed(int speed) override
  {
    m_speed = speed;
  }


 
};

#endif