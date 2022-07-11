#ifndef CLOCK_H
#define CLOCK_H

#pragma once
#include "AccelStepper.h"
#include "IMoveBehaviour.h"
#include "MoveConstant.h"
#include "MoveRandomly.h"
#include "MoveTickTack.h"

class Clock
{
protected:

//AccelStepper
    const byte X_STEPPER_STEP_PIN  {33};
    const byte X_STEPPER_DIR_PIN   {34};

    const byte Y_STEPPER_STEP_PIN  {35};
    const byte Y_STEPPER_DIR_PIN   {30};

    const byte A_STEPPER_STEP_PIN  {32};
    const byte A_STEPPER_DIR_PIN   {31};

    AccelStepper stundenZeiger;
    AccelStepper minutenZeiger;
    AccelStepper sekundenZeiger;

 //Move Behaviour
    IMoveBehaviour *moveBehaviour;
    IMoveBehaviour *moveConstant;
    IMoveBehaviour *moveRandomly;
    IMoveBehaviour *moveTickTack;

public:
    Clock() : stundenZeiger (1, X_STEPPER_STEP_PIN, X_STEPPER_DIR_PIN)
             
    {
        moveConstant = new MoveConstant(stundenZeiger);
        moveRandomly = new MoveRandomly(stundenZeiger);
        moveTickTack = new MoveTickTack(stundenZeiger);
    }

    virtual void setup()
    {
        moveConstant->setup();
        moveRandomly->setup();
        moveTickTack->setup();
    }

    void loop()
    { 
        

        moveConstant->loop();
        moveConstant->setSpeed(900);

        //moveTickTack->loop();
        //moveTickTack->setSpeed(1000);
        

        //setMoveBehaviour(moveConstant);
        //executeMoveBehaviour();
    }

 
 //Set Behaviour on Runtime
    void setMoveBehaviour(IMoveBehaviour *mv)
    {
        this->moveBehaviour = mv;
    }

    void executeMoveBehaviour()
    {
        this->moveBehaviour->loop();
    }
};

#endif