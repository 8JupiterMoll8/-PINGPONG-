#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H
#pragma once

#include "Racket.hpp"
#include "Table.h"
#include "Command.h"
#include "FireCometCommand.h"
#include "CometRaw.h"
#include "LightUpCommand.h"

class InputHandler {

private:

    // Commands
    Command *fireLeftCometCommand;
    Command *fireRightCometCommand;
    Command *lightUpRightTable;
    Command *lightUpLeftTable;

    // Input
    Racket &rightRacket_;
    Racket &leftRacket_;
    Table  &rightTable_;
    Table  &leftTable_;

    // GameObjects
    CometRaw &rightComet_;
    CometRaw &leftComet_;
    CRGB (&ledStrip_)[360];

public:
    // Constructor
    InputHandler(Racket &rightRacket, Racket &leftRacket, Table &rightTable, Table &leftTable, CometRaw &rightComet,
                 CometRaw &leftComet, CRGB (&ledStrip)[360])
            : rightRacket_(rightRacket),
              leftRacket_(leftRacket),
              rightTable_(rightTable),
              leftTable_(leftTable),
              rightComet_(rightComet),
              leftComet_(leftComet), ledStrip_(ledStrip) {
        
        fireLeftCometCommand = new FireCometCommand(leftComet_, leftRacket_);
        fireRightCometCommand = new FireCometCommand(rightComet_, rightRacket_);
        lightUpLeftTable = new LightUpCommand(leftTable_,ledStrip_);
        lightUpRightTable = new LightUpCommand(rightTable_,ledStrip_);

    }

    void loop() {


        if (rightRacket_.isHit()) fireRightCometCommand->execute();
        if (leftRacket_.isHit()) fireLeftCometCommand->execute();
        if (rightTable_.isHit())  lightUpRightTable->execute();
       // if (leftTable_.isHit())   lightUpLeftTable->execute();

         rightComet_.loop();
         leftComet_.loop();


    }

    // Methods to bind commands...
};

#endif