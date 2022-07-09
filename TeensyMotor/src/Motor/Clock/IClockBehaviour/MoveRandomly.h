#ifndef MOVERANDOMLY_H
#define MOVERANDOMLY_H

#pragma once
#include <AccelStepper.h>
#include "IMoveBehaviour.h"

class MoveRandomly : public IMoveBehaviour
{
public:
  MoveRandomly(AccelStepper &Axis) : IMoveBehaviour(Axis)
  {
  }

  virtual void loop() override
  {
    Zeiger.runToNewPosition(0);
    Zeiger.runToNewPosition(500);
  }

  virtual void setSpeed(int speed) override
  {
    m_speed = speed;
  }
};

#endif