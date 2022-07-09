//
// Created by Jupiter Moll on 03.05.2022.
//
#pragma once

#include "PingPong.h"

class LightUpCommand : public Command {

public:
    LightUpCommand(Table &table, CRGB (&ledStrip)[360]) :
            table_(table),
            ledStrip_(ledStrip) {}


public:
    void execute() override {

            int peak = table_.hitPeak();
            static int counter = 0;

            Serial.println(peak);
            Serial.println(counter++);
            allLedsOn();
            usbMIDI.sendNoteOn(72, peak, 10);
          allLedsOff();
         usbMIDI.sendNoteOff(72, 0, 10);

    }

private:
    void allLedsOn() {
        Serial.println("ALL_LEDS_on");
        for (int i = 0; i < 360; ++i) {
            ledStrip_[i] += ledStrip_[i].setRGB(255, 255, 255); // Move Forward

        }
        FastLED.show();
    }

    void allLedsOff() {
        Serial.println("ALL_LEDS_OFF");
        for (int i = 0; i < 360; ++i) {
            ledStrip_[i].nscale8(0);

        }
        FastLED.show();
    }


private:
    Table &table_;
    CRGB (&ledStrip_)[360];


};
