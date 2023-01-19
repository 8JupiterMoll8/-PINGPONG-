#ifndef MOVETICKTACK_H
#define MOVETICKTACK_H

#pragma once
#include <AccelStepper.h>
#include "IMoveBehaviour.h"
#include "elapsedMillis.h"
class MoveTickTack : public IMoveBehaviour
{
private:
    
    int incrementSpeed = 0;
    float in = 4.712;
    float out;
    elapsedMillis ms;


public:
    MoveTickTack(AccelStepper &accelStepper) : IMoveBehaviour(accelStepper)
    {
         m_speed = 1000;
    }

    virtual void move() override
    {

        tickTack();

        in = StepperMotor.currentPosition() * 0.01;
        out = sin(in) * 61.5 + 61.5;
        usbMIDI.sendNoteOn(map(out, 0.0, 127.0, 30.0, 70.0), map(out, 0.0, 127.0, 0.0, 70.0), 11);
        Serial.println(out);
    }

virtual void setSpeed(int speed) override
  {
    m_speed = speed;
  }

private:
    void tickTack()
    {
        static boolean toogle = false;

        if (ms > m_speed)
        {
            ms = 0;
            toogle = !toogle;

            if (toogle == true)
            {
                incrementSpeed = 9000;
                m_speed = 100;
                usbMIDI.sendNoteOff(120, 127, 11);
            }
            else
            {
                incrementSpeed = 0;
                m_speed = 500;
                usbMIDI.sendNoteOn(120, 127, 11);
            }
        }

        StepperMotor.setSpeed(incrementSpeed);
        StepperMotor.runSpeed();
     
    }
};

#endif