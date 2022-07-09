#ifndef I_CLOCK_MOVE_BEHAVIOUR_H
#define I_CLOCK_MOVE_BEHAVIOUR_H
#pragma once
#include "AccelStepper.h"

class IMoveBehaviour
{
public:
  IMoveBehaviour(AccelStepper &Axis) : 
  Zeiger(Axis)
  {}

  void setup()
  {
    Zeiger.setMaxSpeed(12800);
    Zeiger.setSpeed(10000); // had to slow for my motor
    Zeiger.setAcceleration(100.0);

  }

  virtual void loop() = 0;
  virtual void setSpeed(int speed) = 0;

protected:
  AccelStepper &Zeiger;
  int m_speed;
};

#endif