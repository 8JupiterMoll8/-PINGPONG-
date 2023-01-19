#pragma once
#include "PingPong.h"

class IdleState : public BallWechselState
{
private:
    BallwechselStateMachine *fsm_;

public:
    IdleState(BallwechselStateMachine *ballwechselStateMachine) : fsm_(ballwechselStateMachine) {}

public:
    virtual void loop(Racket &leftRacket, Racket &rightRacket, Table &leftTable, Table &rightTable) override
    {


        Serial.println("STARTE SPIEL");
        Serial.println("State_Start.) AUFSCHLAG : Warte dass, der Ball trifft LEFT RACKET");
        Serial.println("State_Start.) AUFSCHLAG : Warte dass, der Ball trifft RIGHT RACKET");

        fsm_->setState(fsm_->startState_);
    }
};
