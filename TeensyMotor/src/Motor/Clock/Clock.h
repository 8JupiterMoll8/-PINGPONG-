#ifndef CLOCK_H
#define CLOCK_H

#pragma once

#include "IMoveBehaviour.h"
#include "AccelStepper.h"

class Clock
{
public:

AccelStepper    m_stepper;

IMoveBehaviour *moveBehaviour;

IMoveBehaviour *moveConstant;
IMoveBehaviour *moveRandom;
IMoveBehaviour *moveTickTack;


    
public:
// Constructor
    Clock(AccelStepper stepper) : 
    m_stepper{stepper},
    moveBehaviour{0}
  
    {
      
    }       


    void setupMoveBehaviour()
    {     
        this->moveBehaviour->setup();       
    }

    void setMoveBehaviour(IMoveBehaviour *mv)  //Set Behaviour on Runtime
    {
        this->moveBehaviour = mv;
    }

    void executeMoveBehaviour()
    {
        this->moveBehaviour->move();
    }

    void setSpeedMoveBehavoiur(int speed)
    {
        this->moveBehaviour->setSpeed(speed);
    }
};

#endif