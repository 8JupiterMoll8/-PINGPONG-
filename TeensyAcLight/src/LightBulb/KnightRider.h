#ifndef KNIGHTRIDER_H
#define KNIGHTRIDER_H

#pragma once

class KnightRider
{
 private:
  unsigned char (&m_AcBulb)[8];
  elapsedMillis  m_ms;
  int intervall = 25;
 
public:
  KnightRider(unsigned char (&acBulb)[8]) : m_AcBulb(acBulb)
  {
  }

  void loop()
  {
       
   static int circle_Counter = 0; 
   static int toggleOnOff    = 0;   
   

    if (m_ms >= intervall)
    {
      m_ms = m_ms - intervall;

      toggleOnOff++; // 1, 2

      if (toggleOnOff == 1 && circle_Counter <= 8)
      {

        m_AcBulb[circle_Counter] = 10; // Light On
        usbMIDI.sendNoteOn(57+circle_Counter,75,5);
      }

      if (toggleOnOff == 2 && circle_Counter <= 8)
      {

         m_AcBulb[circle_Counter] = 75; // Light Off
         usbMIDI.sendNoteOff(57 + circle_Counter,0,5);

        toggleOnOff = 0;
        circle_Counter++;
      }

      if (circle_Counter == 8)
      {
        circle_Counter = 0;
        toggleOnOff = 0;
      }
    }

  }

  void setBrightness(int b)
  {
    // Light Bulb
    for (int i = 0; i < 8; i++)
    {
      m_AcBulb[i] = b;
    }
  }
  void setSpeed(int speed)
  {
    intervall = speed;

  }

  

};

#endif


