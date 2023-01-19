#pragma once

class FireCometCommand : public Command {

private:
    CometRaw &comet_;
    Racket   &racket_;
public:
    FireCometCommand(CometRaw &comet, Racket &racket) : comet_(comet), racket_(racket) {}

public:
    virtual void execute() override {


        float minIn = 0.00;
        float maxIn = 127.0; // Todo : Find right Value to  calibrate the sensitivity
        float speed = racket_.hitPeak();
        //float speed = constrain(racket_.speed(), minIn, maxIn);
       printRacketSpeedAndHitPeak();

        comet_.start();
        comet_.setSpeed(map(speed, minIn, maxIn, 1.0, 10.0));
        comet_.setWidth(map(speed, int(minIn), int(maxIn), 1, 25));
        comet_.setFadeSize(map(speed, minIn, maxIn, 223.0, 134.0));
        comet_.setMidiVelocity(map(speed, minIn, maxIn, 0.0, 127.0));
    }

private:
    void printRacketSpeedAndHitPeak() {
        static int counter = 0;
        Serial.println("Firing Comet");
        Serial.println(counter++);

        Serial.print("Racket-Speed : ");
        Serial.println(racket_.speed());
        Serial.print("Racket-HitPeak : ");
        Serial.println(racket_.hitPeak());
    }
};
