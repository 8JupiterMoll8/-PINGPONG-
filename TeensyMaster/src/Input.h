//
// Created by Jupiter Moll on 02.05.2022.
//

#ifndef PINGPONG2022CLION_INPUT_H
#define PINGPONG2022CLION_INPUT_H


#include "Arduino.h"
#include "Racket/Racket.hpp"
#include "Table/Table.h"

class Input {
public:
    Input(Racket &leftRacket, Racket &rightRacket, Table &leftTable, Table &rightTable) : leftRacket_(leftRacket),
                                                                                          rightRacket_(rightRacket),
                                                                                          leftTable_(leftTable),
                                                                                          rightTable_(rightTable) {}

public:
    Racket &leftRacket_;
    Racket &rightRacket_;
    Table  &leftTable_;
    Table  &rightTable_;


    //Left Racket
    boolean leftRacket_isHit()
    {
        return leftRacket_.isHit();
    }


};


#endif //PINGPONG2022CLION_INPUT_H
