#ifndef CLOCKER_H
#define CLOCKER_H

#pragma once
#include "IMoveBehaviour.h"


class Clocker
{
private: IMoveBehaviour *moveBehaviour;
                                                                                                    
    
public: Clocker() : moveBehaviour{0}{}       


public: void setupMoveBehaviour() {     

        this->moveBehaviour->setup();       
    }

public: void setMoveBehaviour(IMoveBehaviour *mv) {

        this->moveBehaviour = mv;
    }

public: void executeMoveBehaviour() {

        this->moveBehaviour->move();
    }

public: void setSpeedMoveBehavoiur(int speed) {
        
        this->moveBehaviour->setSpeed(speed);
    }

};

#endif