#ifndef CHARGECOMMAND_H
#define CHARGECOMMAND_H

#pragma once
#include "PingPong.h"
#include "elapsedMillis.h"


class ChargeCommand : public Command
{
public:
    ChargeCommand(Racket &racket, byte midiChannel) :  m_racket(racket) , m_midiChannel(midiChannel) {}

private:
    Racket &m_racket;
    byte m_midiChannel = 3;
    int m_pressure;
    elapsedMillis ms;

     // store previously sent values, to detect changes
  int previousA0 = -1;
  int previousA1 = -1;

public: 
   virtual void execute() override {

        pressureSensorMidiNoteSend();
   
    }


    void pressureSensorMidiNoteSend()
    {
         static boolean toogle = false;

       m_pressure = m_racket.pressure();
       m_pressure = map(m_pressure,0,1024,0,127);


       if (ms > 10)
        {
            ms = 0;
            toogle = !toogle;

            if (toogle == true)
            {
               
                usbMIDI.sendNoteOff(m_pressure, 0, m_midiChannel);
               
            }
            else
            {
              
                usbMIDI.sendNoteOn(m_pressure, 127, m_midiChannel);
            }
        }

         allnotesOff();
         

    }

    void allnotesOff(){
        if(m_pressure<10){
            for (int i = 0; i < 127; i++)
            {
                usbMIDI.sendNoteOff(i, 0, m_midiChannel);

            }
            
        }
    }


    
    


};

#endif