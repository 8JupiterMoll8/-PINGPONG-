
#pragma once
#include "PingPong.h"

class A_Left_Table_LinksAufschlagState : public BallWechselState
{
private:
    BallwechselStateMachine *fsm_;

public:
    A_Left_Table_LinksAufschlagState(BallwechselStateMachine *ballwechselStateMachine) : fsm_(ballwechselStateMachine) {}

public:
    virtual void loop(Racket &leftRacket, Racket &rightRacket, Table &leftTable, Table &rightTable) override
    {
           if (leftRacket.isHit() || rightRacket.isHit()|| rightTable.isHit() ){
                    Serial.println("!!!!!!!!!!!!!!!!DOPPelFEHler!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

                  
                     fsm_->setState(fsm_->startState_);
                   
                }
      
      if(leftRacket.isHit())
      {
            Serial.println("LINKS_AUFSCHLAG : BAll trifft Left Racket");
            Serial.println("State 2.)AUFSCHLAG : Warte das BAll trifft LEFT TABLE");
      }
      

        
    }
};
