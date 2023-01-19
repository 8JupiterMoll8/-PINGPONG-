
#pragma once

#include "PingPong.h"

class BallWechselCounter {

private:
    enum States {
        /*00*/IDLE,

        /*01*/LINKS_AUFSCHLAG_LEFT_RACKET_oder_RECHTS_AUFSCHLAG_RIGHT_RACKET,

        /*02*/LINKS_AUFSCHLAG_LEFT_TABLE,
        /*03*/LINKS_AUFSCHLAG_RIGHT_TABLE,
        /*04*/LINKS_BALLWECSHEL_RIGHT_RACKET,
        /*05*/LINKS_BALLWECHSEL_LEFT_TABLE,
        /*06*/LINKS_BALLWECHSEL_LEFT_RACKET,
        /*07*/LINKS_BALLWECHSEL_RIGHT_TABLE,
        /*08*/LINKS_BALLWECHSEL_ERFOLGREICH_RIGHT_RACKET,
        /*09*/LINKS_BALLWECHSEL_COUNT,

        /*10*/RECHTS_AUFSCHLAG_RIGHT_TABLE,
        /*11*/RECHTS_AUFSCHLAG_LEFT_TABLE,
        /*12*/RECHTS_BALLWECHSEL_LEFT_RACKET,
        /*13*/RECHTS_BALLWECHSEL_RIGHT_TABLE,
        /*14*/RECHTS_BALLWECSHEL_RIGHT_RACKET,
        /*15*/RECHTS_BALLWECHSEL_LEFT_TABLE,
        /*16*/RECHTS_BALLWECHSEL_ERFOLGREICH_LEFT_RACKET,
        /*17*/RECHTS_BALLWECHSEL_COUNT,

        /*18*/DOPPEL_FEHLER,
        /*19*/FINISH_GAME
    };

public:
    BallWechselCounter(Racket &leftRacket, Racket &rightRacket, Table &leftTable, Table &rightTable) :
            leftRacket_(leftRacket), rightRacket_(rightRacket), leftTable_(leftTable), rightTable_(rightTable) {

        totalBallWechsel_ = 0;
        state_ = IDLE;
        lastState_ = 0;
    }

    byte getTotalBallwechsel() {
        return totalBallWechsel_;

    }

public:
    void loop() {
        switch (state_) {

            case IDLE:
                state_ = LINKS_AUFSCHLAG_LEFT_RACKET_oder_RECHTS_AUFSCHLAG_RIGHT_RACKET;
                break;

            case LINKS_AUFSCHLAG_LEFT_RACKET_oder_RECHTS_AUFSCHLAG_RIGHT_RACKET:
                if (leftRacket_.isHit()) state_ = LINKS_AUFSCHLAG_LEFT_TABLE;
                if (rightRacket_.isHit()) state_ = RECHTS_AUFSCHLAG_RIGHT_TABLE;
                break;

            case LINKS_AUFSCHLAG_LEFT_TABLE:
                if (leftTable_doppelFehler()) state_ = DOPPEL_FEHLER;
                if (leftTable_.isHit())       state_ = LINKS_AUFSCHLAG_RIGHT_TABLE;
                break;

            case LINKS_AUFSCHLAG_RIGHT_TABLE:
                if (rightTable_doppelFehler()) state_ = DOPPEL_FEHLER;
                if (rightTable_.isHit())       state_ = LINKS_BALLWECSHEL_RIGHT_RACKET;
                break;

            case LINKS_BALLWECSHEL_RIGHT_RACKET:
                if (rightRacket_doppelFehler()) state_ = DOPPEL_FEHLER;
                if (rightRacket_.isHit())       state_ = LINKS_BALLWECHSEL_LEFT_TABLE;
                break;

            case LINKS_BALLWECHSEL_LEFT_TABLE:
                if (leftTable_doppelFehler()) state_ = DOPPEL_FEHLER;
                if (leftTable_.isHit())       state_ = LINKS_BALLWECHSEL_LEFT_RACKET;
                break;

            case LINKS_BALLWECHSEL_LEFT_RACKET:
                if (leftRacket_doppelFehler()) state_ = DOPPEL_FEHLER;
                if (leftRacket_.isHit()) state_ = LINKS_BALLWECHSEL_RIGHT_TABLE;
                break;

            case LINKS_BALLWECHSEL_RIGHT_TABLE:
                if (rightTable_doppelFehler()) state_ = DOPPEL_FEHLER;
                if (rightTable_.isHit()) state_ = LINKS_BALLWECHSEL_ERFOLGREICH_RIGHT_RACKET;
                break;

            case LINKS_BALLWECHSEL_ERFOLGREICH_RIGHT_RACKET:
                if (rightRacket_doppelFehler()) state_ = DOPPEL_FEHLER;
                if (rightRacket_.isHit()) state_ = LINKS_BALLWECHSEL_COUNT;
                break;

            case LINKS_BALLWECHSEL_COUNT:
                ++totalBallWechsel_;
                state_ = LINKS_BALLWECHSEL_LEFT_TABLE;
                break;


            case RECHTS_AUFSCHLAG_RIGHT_TABLE:
                if (rightTable_doppelFehler()) state_ = DOPPEL_FEHLER;
                if (rightTable_.isHit()) state_ = RECHTS_AUFSCHLAG_LEFT_TABLE;
                break;

            case RECHTS_AUFSCHLAG_LEFT_TABLE:
                if (leftTable_doppelFehler()) state_ = DOPPEL_FEHLER;
                if (leftTable_.isHit())state_ = RECHTS_BALLWECHSEL_LEFT_RACKET;
                break;

            case RECHTS_BALLWECHSEL_LEFT_RACKET:
                if (leftRacket_doppelFehler()) state_ = DOPPEL_FEHLER;
                if (leftRacket_.isHit())state_ = RECHTS_BALLWECHSEL_RIGHT_TABLE;
                break;

            case RECHTS_BALLWECHSEL_RIGHT_TABLE:
                if (rightTable_doppelFehler()) state_ = DOPPEL_FEHLER;
                if (rightTable_.isHit())state_ = RECHTS_BALLWECSHEL_RIGHT_RACKET;

                break;
            case RECHTS_BALLWECSHEL_RIGHT_RACKET:
                if (rightRacket_doppelFehler()) state_ = DOPPEL_FEHLER;
                if (rightRacket_.isHit())state_ = RECHTS_BALLWECHSEL_LEFT_TABLE;

                break;
            case RECHTS_BALLWECHSEL_LEFT_TABLE:
                if (leftTable_doppelFehler()) state_ = DOPPEL_FEHLER;
                if (leftTable_.isHit()) state_ = RECHTS_BALLWECHSEL_ERFOLGREICH_LEFT_RACKET;
                break;

            case RECHTS_BALLWECHSEL_ERFOLGREICH_LEFT_RACKET:
                if (leftRacket_doppelFehler()) state_ = DOPPEL_FEHLER;
                if (leftRacket_.isHit()) state_ = RECHTS_BALLWECHSEL_COUNT;
                break;

            case RECHTS_BALLWECHSEL_COUNT:
                ++totalBallWechsel_;
                state_ = RECHTS_BALLWECHSEL_LEFT_TABLE;
                break;

            case DOPPEL_FEHLER:
                state_ = IDLE;
                break;

            case FINISH_GAME:

                break;
        } // end switch

    }

private: // DoppelFehler
    boolean leftRacket_doppelFehler() {
        if (leftTable_.isHit() || rightRacket_.isHit() || leftTable_.isHit())
            return true;
        else
            return false;
    }

    boolean rightRacket_doppelFehler() {
        if (leftRacket_.isHit() || rightTable_.isHit() || leftTable_.isHit())
            return true;
        else
            return false;
    }

    boolean rightTable_doppelFehler() {
        if (leftRacket_.isHit() || rightRacket_.isHit() || leftTable_.isHit())
            return true;
        else
            return false;
    }

    boolean leftTable_doppelFehler() {
        if (leftRacket_.isHit() || rightRacket_.isHit() || rightTable_.isHit())
            return true;
        else
            return false;
    }


private:
    Racket &leftRacket_;
    Racket &rightRacket_;
    Table &leftTable_;
    Table &rightTable_;


    int totalBallWechsel_;

private:

public:
    void printDebug() {

        if (lastState_ != state_) {
            Serial.print("****************** State : ");
            Serial.print(state_);
            Serial.println(" ****************");

            if (state_ == 1) {
                Serial.println("Start - Ballwechsel - Game");
                Serial.println("LINKS_AUFSCHLAG oder RECHTS_AUFSCHLAG");
            }

            if (state_ == 2) {
                Serial.println("LINKS_AUFSCHLAG   : HIT_LEFT_RACKET");
                Serial.println("LINKS_AUFSCHLAG   : Waiting for HIT_LEFT_TABLE");
            }

            if (state_ == 3) {
                Serial.println("LINKS_AUFSCHLAG   : HIT_LEFT_TABLE");
                Serial.println("LINKS_AUFSCHLAG   : Waiting for HIT_RIGHT_TABLE");
            }

            if (state_ == 4) {
                Serial.println("LINKS_BALLWECHSEL : HIT_RIGHT_TABLE");
                Serial.println("LINKS_BALLWECHSEL : Waiting for HIT_RIGHT_RACKET");
            }

            if (state_ == 5) {
                Serial.println("LINKS_BALLWECHSEL : HIT_RIGHT_RACKET");
                Serial.println("LINKS_BALLWECHSEL : Waiting for HIT_LEFT_TABLE");
            }

            if (state_ == 6) {
                Serial.println("LINKS_BALLWECHSEL : HIT_LEFT_TABLE");
                Serial.println("LINKS_BALLWECHSEL : Waiting for HIT_LEFT_RACKET");
            }

            if (state_ == 7) {
                Serial.println("LINKS_BALLWECHSEL : HIT_LEFT_RACKET");
                Serial.println("LINKS_BALLWECHSEL : Waiting for HIT_RIGHT_TABLE");
            }

            if (state_ == 8) {
                Serial.println("LINKS_BALLWECHSEL : HIT_RIGHT_TABLE");
                Serial.println("LINKS_BALLWECHSEL : Waiting for HIT_RIGHT_RACKET");
            }

            if (state_ == 9) {
                Serial.println("LINKS_BALLWECHSEL : HIT_RIGHT_RACKET");
                Serial.println("LINKS_BALLWECHSEL : ERFOLGREICH !!!");
                Serial.print("LINKS_BALLWECHSEL : Total BallWechsel = ");
                Serial.println(totalBallWechsel_);
                Serial.println("LINKS_BALLWECHSEL : Waiting for HIT_LEFT_TABLE");
            }

            if (state_ == 10) {
                Serial.println("RECHTS_AUFSCHLAG   : HIT_RIGHT_RACKET");
                Serial.println("RECHTS_AUFSCHLAG   : Waiting for HIT_RIGHT_TABLE");
            }

            if (state_ == 11) {
                Serial.println("RECHTS_AUFSCHLAG   : HIT_RIGHT_TABLE");
                Serial.println("RECHTS_AUFSCHLAG   : Waiting for HIT_LEFT_TABLE");
            }

            if (state_ == 12) {
                Serial.println("RECHTS_BALLWECHSEL : HIT_LEFT_TABLE");
                Serial.println("RECHTS_BALLWECHSEL : Waiting for HIT_LEFT_RACKET");
            }

            if (state_ == 13) {
                Serial.println("LINKS_BALLWECHSEL : HIT_LEFT_RACKET");
                Serial.println("LINKS_BALLWECHSEL : Waiting for HIT_RIGHT_TABLE");
            }

            if (state_ == 14) {
                Serial.println("LINKS_BALLWECHSEL : HIT_RIGHT_TABLE");
                Serial.println("LINKS_BALLWECHSEL : Waiting for HIT_RIGHT_RACKET");
            }

            if (state_ == 15) {
                Serial.println("LINKS_BALLWECHSEL : HIT_RIGHT_RACKET");
                Serial.println("LINKS_BALLWECHSEL : Waiting for HIT_LEFT_TABLE");
            }

            if (state_ == 16) {
                Serial.println("RECHTS_BALLWECHSEL : HIT_LEFT_TABLE");
                Serial.println("RECHTS_BALLWECHSEL : Waiting for HIT_LEFT_RACKET");
            }

            if (state_ == 17) {
                Serial.println("RECHTS_BALLWECHSEL : HIT_LEFT_RACKET");
                Serial.println("RECHTS_BALLWECHSEL : ERFOLGREICH !!!");
                Serial.print("RECHTS_BALLWECHSEL : Total BallWechsel = ");
                Serial.println(totalBallWechsel_);
                Serial.println("RECHTS_BALLWECHSEL : Waiting for HIT_RIGHT_TABLE");
            }


            if (state_ == 18) {
                Serial.println("DOPPELFEHLLER");
            }

            lastState_ = state_;
        }
    }// End loop

private:
    States state_;
    byte lastState_;

};
