#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#pragma once
#include "CometRaw.h"

class CollisionDetector
{
    CometRaw &m_leftComet;
    CometRaw &m_rightComet;

public:
    CollisionDetector(CometRaw &leftComet, CometRaw &rightComet) : m_leftComet(leftComet),
                                                                   m_rightComet(rightComet)
    {
    }

public:
    boolean isCollision()
    {
      if ( 
          m_leftComet.xPos() < m_rightComet.xPos() ||  // if leftComet is inside right Side of rigjt Comrt
          m_rightComet.xPos() > m_leftComet.xPos()
         )
         {   
             Serial.println("Collision");

             m_leftComet.setColor(255,0,0);
             m_leftComet.reverseDirection();
             return true;
         }else
         {
             return false;
         }
    }

  
     
};
#endif