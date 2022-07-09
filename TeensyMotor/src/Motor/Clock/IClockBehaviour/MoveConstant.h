#ifndef CLOCK_CONSTANT_MOVE_H
#define CLOCK_CONSTANT_MOVE_H

#pragma once
#include <AccelStepper.h>
#include "IMoveBehaviour.h"
class MoveConstant : public IMoveBehaviour
{

public:
  MoveConstant(AccelStepper &Axis) : 
  IMoveBehaviour(Axis)
  {
      m_speed = 4000;
  }

  virtual void loop() override
  {
    Zeiger.setSpeed(m_speed);
    Zeiger.runSpeed();
    //Zeiger.runNow();
    
  
  }

  virtual void setSpeed(int speed) override
  {
    m_speed = speed;
  }


 
};

#endif