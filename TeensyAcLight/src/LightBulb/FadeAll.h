#ifndef FADERALL_H
#define FADERALL_H

#pragma once
#include "Arduino.h"
class FadeAll
{
private:
    unsigned char (&m_leftBulbs)[8];
    unsigned char (&m_rightBulbs)[8];

public:
    FadeAll(unsigned char (&leftBulbs)[8], unsigned char (&rightBulbs)[8]) : m_leftBulbs(leftBulbs),
                                                                           m_rightBulbs(rightBulbs)
    {
    }

    void loop()
    {

        static float in = 4.712;
        static float out;
        //  if( ms > 1)
        //  {
        // ms = 0;

        in = in + 0.000005;
        if (in > 10.995)
            in = 4.712;
        out = sin(in) * 35 + 35;

        //  }

        for (int i = 0; i < 8; i++)
        {
            m_leftBulbs[i] = map(out, 0, 70, 10, 70);
            m_rightBulbs[i] = map(out, 0, 70, 10, 70);
            usbMIDI.sendNoteOn(map(out, 0, 70, 70, 10), 72, 5);
        }
    }
};
#endif