
#pragma once
#include "PingPong.h"

class StartState : public BallWechselState
{
private:
    BallwechselStateMachine *fsm_;

public:
    StartState(BallwechselStateMachine *ballwechselStateMachine) : fsm_(ballwechselStateMachine) {}

public:
    virtual void loop(Racket &leftRacket, Racket &rightRacket, Table &leftTable, Table &rightTable) override
    {
      
      if(leftRacket.isHit())
      {
          fsm_->setState(fsm_->A_lt_LinksAufschlagState_);
          Serial.println("LINKS_AUFSCHLAG : BAll trifft Left Racket");
          Serial.println("State 2.)AUFSCHLAG : Warte das BAll trifft LEFT TABLE");

      }
      

        
    }
};

