#pragma once
#include "PingPong.h"

class BallwechselStateMachine
{
public:
// Input
    Racket &leftRacket_;
    Racket &rightRacket_;
    Table &leftTable_;
    Table &rightTable_;

// States
    BallWechselState *idleState_;
    BallWechselState *startState_;
    BallWechselState *A_lt_LinksAufschlagState_;
    
    

    BallWechselState *state_;

// Constructor
    BallwechselStateMachine(Racket &leftRacket, Racket &rightRacket, Table &leftTable, Table &rightTable);

// Change your internal Behaviour
    void setState(BallWechselState *state)
    {
        state_ = state;
    }

// Update StateMAchine and Delegate
    void loop()
    {
        state_->loop(leftRacket_, rightRacket_, leftTable_, rightTable_);
    }
};
