#include <Arduino.h>
//#include "TimerOne.h"
//#include "Birnen.hpp"
//#include "KnightRider.h"
//#include "FadeAll.h"

#include <EasyTransfer.h>
#include "PingPongProtocol.h"

/*
███████╗ █████╗ ███████╗██╗   ██╗████████╗██████╗  █████╗ ███╗   ██╗███████╗███████╗███████╗██████╗ 
██╔════╝██╔══██╗██╔════╝╚██╗ ██╔╝╚══██╔══╝██╔══██╗██╔══██╗████╗  ██║██╔════╝██╔════╝██╔════╝██╔══██╗
█████╗  ███████║███████╗ ╚████╔╝    ██║   ██████╔╝███████║██╔██╗ ██║███████╗█████╗  █████╗  ██████╔╝
██╔══╝  ██╔══██║╚════██║  ╚██╔╝     ██║   ██╔══██╗██╔══██║██║╚██╗██║╚════██║██╔══╝  ██╔══╝  ██╔══██╗
███████╗██║  ██║███████║   ██║      ██║   ██║  ██║██║  ██║██║ ╚████║███████║██║     ███████╗██║  ██║
╚══════╝╚═╝  ╚═╝╚══════╝   ╚═╝      ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝╚═╝     ╚══════╝╚═╝  ╚═╝
                                                                                                    
*/

EasyTransfer ET_Light;
pingpong::WorldFrame worldFrame{};

constexpr uint8_t rightRacketHitPin{22};
constexpr uint32_t communicationTimeoutMs{500};
constexpr uint32_t hitPulseMs{10};
uint32_t lastValidFrameMs{0};
uint32_t hitPulseStartedMs{0};
uint16_t lastFrameSequence{0};
uint8_t lastLeftRacketHitCount{0};
uint8_t lastRightRacketHitCount{0};
uint8_t lastLeftTableHitCount{0};
uint8_t lastRightTableHitCount{0};
int currentRoll{0};
bool hasValidFrame{false};
bool hitPulseActive{false};


/*
██╗     ██╗ ██████╗ ██╗  ██╗████████╗
██║     ██║██╔════╝ ██║  ██║╚══██╔══╝
██║     ██║██║  ███╗███████║   ██║
██║     ██║██║   ██║██╔══██║   ██║
███████╗██║╚██████╔╝██║  ██║   ██║
╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝

*/


// KnightRider l_knightRider(l_CH);
// KnightRider r_knightRider(CH);
// FadeAll fadeAll(l_CH,CH);


void setup() {
  Serial.begin(3000000);
  Serial8.begin(6000000);
  ET_Light.begin(details(worldFrame), &Serial8);

  // INIT AC BULBS 240V PHASE CONTROLLER
  //setup_Dimmer();
  Serial.println("AC DIMMER 2");
  
  pinMode(rightRacketHitPin, OUTPUT);
  digitalWrite(rightRacketHitPin, LOW);


}

void loop() 
{
//AC 240V BULBS

 //l_knightRider.loop();
 //l_knightRider.setSpeed(1000);
 //l_knightRider.setBrightness(30);

 //r_knightRider.loop();
 //r_knightRider.setSpeed(1000);
 //r_knightRider.setBrightness(30);

 //fadeAll.loop();



  const uint32_t now = millis();
  if (ET_Light.receiveData()
      && pingpong::isCompatible(worldFrame)
      && (!hasValidFrame || worldFrame.frameSequence != lastFrameSequence)) {
    const bool rightRacketHit = hasValidFrame && worldFrame.rightRacketHitCount != lastRightRacketHitCount;
#ifdef PINGPONG_LINK_DEBUG
    const bool leftRacketHit = hasValidFrame && worldFrame.leftRacketHitCount != lastLeftRacketHitCount;
    const bool leftTableHit = hasValidFrame && worldFrame.leftTableHitCount != lastLeftTableHitCount;
    const bool rightTableHit = hasValidFrame && worldFrame.rightTableHitCount != lastRightTableHitCount;
#endif

    hasValidFrame = true;
    lastValidFrameMs = now;
    lastFrameSequence = worldFrame.frameSequence;
    lastLeftRacketHitCount = worldFrame.leftRacketHitCount;
    lastRightRacketHitCount = worldFrame.rightRacketHitCount;
    lastLeftTableHitCount = worldFrame.leftTableHitCount;
    lastRightTableHitCount = worldFrame.rightTableHitCount;

    const float roll = constrain(worldFrame.leftRacketRoll, -180.0F, 180.0F);
    currentRoll = static_cast<int>((roll + 180.0F) * 9000.0F / 360.0F);

    if (pingpong::isRunning(worldFrame) && rightRacketHit) {
      digitalWrite(rightRacketHitPin, HIGH);
      hitPulseStartedMs = now;
      hitPulseActive = true;
    }

#ifdef PINGPONG_LINK_DEBUG
    if (rightTableHit) Serial.println("HitRightTable");
    if (leftTableHit) Serial.println("HitLeftTable");
    if (rightRacketHit) Serial.println("HitRightRacket");
    if (leftRacketHit) Serial.println("HitLeftRacket");
#endif
  }

  if (hitPulseActive && now - hitPulseStartedMs >= hitPulseMs) {
    digitalWrite(rightRacketHitPin, LOW);
    hitPulseActive = false;
  }

  const bool communicationTimedOut = !hasValidFrame || now - lastValidFrameMs >= communicationTimeoutMs;
  if (communicationTimedOut || !pingpong::isRunning(worldFrame)) {
    currentRoll = 0;
    digitalWrite(rightRacketHitPin, LOW);
    hitPulseActive = false;
    hasValidFrame = false;
  }





}
