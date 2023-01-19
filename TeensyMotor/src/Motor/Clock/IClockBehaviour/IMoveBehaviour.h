#ifndef I_CLOCK_MOVE_BEHAVIOUR_H
#define I_CLOCK_MOVE_BEHAVIOUR_H
#pragma once
#include "AccelStepper.h"

class IMoveBehaviour
{
public:
  IMoveBehaviour(AccelStepper &accelStepper) : StepperMotor(accelStepper)
  {
    
  }

  void setup()
  {
    StepperMotor.setMaxSpeed(12800);
    StepperMotor.setSpeed(10000); // had to slow for my motor
    StepperMotor.setAcceleration(100.0);

  }

  virtual void move() = 0;
  virtual void setSpeed(int speed) = 0;

protected:
  AccelStepper &StepperMotor;
  int m_speed;
};

#endif