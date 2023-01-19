#ifndef MOVERANDOMLY_H
#define MOVERANDOMLY_H

#pragma once
#include <AccelStepper.h>
#include "IMoveBehaviour.h"

class MoveRandomly : public IMoveBehaviour
{
public:
  MoveRandomly(AccelStepper &accelStepper) : IMoveBehaviour(accelStepper)
  {
  }

  virtual void move() override
  {
    StepperMotor.runToNewPosition(0);
    StepperMotor.runToNewPosition(500);
  }

  virtual void setSpeed(int speed) override
  {
    m_speed = speed;
  }
};

#endif