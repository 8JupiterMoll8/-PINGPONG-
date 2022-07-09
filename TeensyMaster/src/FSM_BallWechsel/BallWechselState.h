#pragma once
#include "PingPong.h"

class BallWechselState
{
  
public:
    virtual void loop(Racket &leftRacket, Racket &rightRacket, Table &leftTable, Table &rightTable) = 0;
};
