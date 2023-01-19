#ifndef RACKET
#define RACKET
#pragma once

#include "Piezo.hpp"
#include "Speed.hpp"
#include "Swing.hpp"
#include "Mahony.hpp"
#include "Pressure.hpp"

class Racket {
private:
    //Hit Behaviour
    Piezo &piezo_;
    //Motion Behaviour
    Speed &_speed;
    Swing &_swing;
    Mahony &_mahony;
    //Button Behaviour
    Pressure &m_pressure;


public:
    Racket(Piezo &piezo, Speed &speed, Swing &swing, Mahony &mahony, Pressure &pressure) :
            piezo_(piezo),
            _speed(speed),
            _swing(swing),
            _mahony(mahony),
            m_pressure(pressure) {

    }

    // LOOP
    auto loop() {
        piezo_.loop();
        _swing.loop();
        _speed.loop();
        _mahony.loop();
        m_pressure.loop();
    }

    /*BEHAVIOURS*/
    //SPEED
    auto speed() { return _speed.getTotal(); }

    //SWING
    auto swing_A_Gain() { return _swing.A_Gain(); }

    auto swing_B_Gain() { return _swing.B_Gain(); }

    //HIT
    auto isHit() { return piezo_.isHit(); }

    auto hitPeak() { return piezo_.hitPeak(); }

    auto hitSum() { return piezo_.hitSum(); }

    auto resetHitSum() { piezo_.resetHitSum(); }

    //ROLL PITCH YAW
    auto roll() { return _mahony.roll(); }

    auto pitch() { return _mahony.pitch(); }

    auto yaw() { return _mahony.yaw(); }

    //PRESSURE BUTTON
    auto pressure() { return m_pressure.value(); }
    auto isPress()  {return m_pressure.isPressed();}
    // Debug ()

    auto print ()
      {
        if(piezo_.isHit()){
            Serial.println( piezo_.hitSum());
        }

      }

};

#endif /* RACKET */
