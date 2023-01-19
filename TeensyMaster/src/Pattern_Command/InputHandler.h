#ifndef INPUTHANDLER
#define INPUTHANDLER
#pragma once

#include "Racket.hpp"
#include "Table.h"
#include "Command.h"
#include "FireCometCommand.h"
#include "CometRaw.h"
#include "LightUpCommand.h"
#include "ChargeCommand.h"
#include "SwingCommand.h"
#include "RollPitchYawCommand.h"

class InputHandler {

private:

    // Input
    Racket &rightRacket_;
    Racket &leftRacket_;
    Table  &rightTable_;
    Table  &leftTable_;

    // Commands
    Command *fireLeftCometCommand;
    Command *fireRightCometCommand;
    Command *lightUpRightTable;
    Command *lightUpLeftTable;
    Command *chargeLeftRacket;
    Command *chargeRightRacket;
    Command *swingLeftRacket;
    Command *swingRightRacket;
    Command *rollPitchYawLeftRacket;
    Command *rollPitchYawRightRacket;


    // GameObjects
    CometRaw &rightComet_;
    CometRaw &leftComet_;
    CRGB (&ledStrip_)[58];

public:
    // Constructor
    InputHandler(Racket &rightRacket, Racket &leftRacket, Table &rightTable, Table &leftTable, CometRaw &rightComet,
                 CometRaw &leftComet, CRGB (&ledStrip)[58])

            : rightRacket_(rightRacket),
              leftRacket_ (leftRacket),
              rightTable_ (rightTable),
              leftTable_  (leftTable),
              rightComet_ (rightComet),
              leftComet_  (leftComet), 
              ledStrip_   (ledStrip) 
    {
        
        fireLeftCometCommand  = new FireCometCommand(leftComet_, leftRacket_);
        fireRightCometCommand = new FireCometCommand(rightComet_, rightRacket_);

        lightUpLeftTable      = new LightUpCommand (leftTable_,ledStrip_);
        lightUpRightTable     = new LightUpCommand  (rightTable_,ledStrip_);

        chargeRightRacket     = new ChargeCommand (rightRacket_,5);
        chargeLeftRacket      = new ChargeCommand(leftRacket_,8);
        
        swingRightRacket      = new SwingCommand(rightRacket_,2);
        swingLeftRacket       = new SwingCommand(leftRacket_,1);

        rollPitchYawLeftRacket  = new RollPitchYawCommand(leftRacket_,3);
        rollPitchYawRightRacket = new RollPitchYawCommand(rightRacket_,4);

    }

    void loop() {


        if (rightRacket_.isHit()) fireRightCometCommand->execute();
        if (leftRacket_.isHit())  fireLeftCometCommand->execute();

        if (rightTable_.isHit())  lightUpRightTable->execute();
        if (leftTable_.isHit())   lightUpLeftTable->execute();

        if(rightRacket_.isPress())chargeRightRacket->execute();
        if(leftRacket_.isPress()) chargeLeftRacket->execute();

        // FSR Pressure Sensor Active Roll PItch YAW
        //! Todo set Thershold for IsPress
        if(rightRacket_.isPress())rollPitchYawRightRacket->execute();
        if(leftRacket_.isPress()) rollPitchYawLeftRacket->execute();

         swingLeftRacket->execute();
         swingRightRacket->execute();

        // rightComet_.loop();
        // leftComet_.loop();


    }

    // Methods to bind commands...
};

#endif /* INPUTHANDLER */
