#ifndef CLOCK_H
#define CLOCK_H

#pragma once

#include "IMoveBehaviour.h"
#include "AccelStepper.h"

class Clock
{
public:


int m_pinStep;
int m_pinDir;  
 //Move Behaviour
AccelStepper stepper;
IMoveBehaviour *moveBehaviour;

    
public:
// Constructor
    Clock(int pinStep, int pinDir) : 
    m_pinStep {pinStep},
    m_pinDir  {pinDir},
    stepper  (1, m_pinStep, m_pinDir),
    moveBehaviour(0)
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