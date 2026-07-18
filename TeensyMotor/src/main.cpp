#include <Arduino.h>
#include "AccelStepper.h"
#include "LeftClock.h"
#include "Clocker.h"
#include "IMoveBehaviour.h"
#include "MoveTickTack.h"
#include "MoveRandomly.h"
#include "MoveConstant.h"
#include "PingPongProtocol.h"

#include <EasyTransfer.h>
#include <Wire.h>
#include <EasyTransferI2C.h>
#include "elapsedMillis.h"
/*
███████╗ █████╗ ███████╗██╗   ██╗████████╗██████╗  █████╗ ███╗   ██╗███████╗███████╗███████╗██████╗ 
██╔════╝██╔══██╗██╔════╝╚██╗ ██╔╝╚══██╔══╝██╔══██╗██╔══██╗████╗  ██║██╔════╝██╔════╝██╔════╝██╔══██╗
█████╗  ███████║███████╗ ╚████╔╝    ██║   ██████╔╝███████║██╔██╗ ██║███████╗█████╗  █████╗  ██████╔╝
██╔══╝  ██╔══██║╚════██║  ╚██╔╝     ██║   ██╔══██╗██╔══██║██║╚██╗██║╚════██║██╔══╝  ██╔══╝  ██╔══██╗
███████╗██║  ██║███████║   ██║      ██║   ██║  ██║██║  ██║██║ ╚████║███████║██║     ███████╗██║  ██║
╚══════╝╚═╝  ╚═╝╚══════╝   ╚═╝      ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝╚═╝     ╚══════╝╚═╝  ╚═╝
                                                                                                    
*/

EasyTransfer ET_Motor;
pingpong::WorldFrame worldFrame{};
EasyTransferI2C ET_Ic2; 
void receive(int numBytes);

constexpr uint32_t communicationTimeoutMs{500};
constexpr int maximumMotorSpeed{9000};
uint32_t lastValidFrameMs{0};
uint16_t lastFrameSequence{0};
uint8_t lastLeftRacketHitCount{0};
uint8_t lastRightRacketHitCount{0};
uint8_t lastLeftTableHitCount{0};
uint8_t lastRightTableHitCount{0};
bool hasValidFrame{false};


/*
 ██████╗██╗      ██████╗  ██████╗██╗  ██╗
██╔════╝██║     ██╔═══██╗██╔════╝██║ ██╔╝
██║     ██║     ██║   ██║██║     █████╔╝ 
██║     ██║     ██║   ██║██║     ██╔═██╗ 
╚██████╗███████╗╚██████╔╝╚██████╗██║  ██╗
 ╚═════╝╚══════╝ ╚═════╝  ╚═════╝╚═╝  ╚═╝
*/  

// AccelStepper leftStepper(1,31,32);
// AccelStepper rightStepper(1,30,34);                              
// LeftClock leftClock(leftStepper);
// LeftClock rightClock(rightStepper);


AccelStepper leftStepper(1,31,32);
Clock motorClock(leftStepper);
MoveConstant moveConstant(leftStepper);
int currentRoll{0};


// Clocker leftClocker;
// AccelStepper leftStepper(1,31,32);
// MoveConstant moveConstant(leftStepper);
// MoveTickTack moveTickTack(leftStepper);
// MoveRandomly moveRandom(leftStepper);

// Clocker rightClocker;
// AccelStepper rightStepper(1,30,34);                                                      
// MoveConstant left_moveConstant(rightStepper);
// MoveTickTack left_moveTickTack(rightStepper);
// MoveRandomly left_moveRandom(rightStepper);





//RightClock rightClock;

void setup() {
  Serial.begin(115200);
  Serial1.begin(6000000);    // EasyTransfer
//   while(!Serial )
//    {  
//     Serial.println("TEENSY-MOTOR");
//  }


    ET_Motor.begin(details(worldFrame), &Serial1);

    // Wire2.begin(9);
    //ET_Ic2.begin(details(mydata), &Wire2);   
    //Wire2.onReceive(receive);



 //leftClock.setup();
 //rightClock.setup();
 
 motorClock.setMoveBehaviour(&moveConstant);
 motorClock.setupMoveBehaviour();




}

void loop() 
{

   //!TH
  //  leftClocker.setMoveBehaviour(&moveConstant); // Init has to before setup
  //  leftClocker.setupMoveBehaviour();
  //  leftClocker.setSpeedMoveBehavoiur(1000);
  //  leftClocker.executeMoveBehaviour();

  //  rightClocker.setMoveBehaviour(&left_moveConstant); // Init has to before setup
  //  rightClocker.setupMoveBehaviour();
  //  rightClocker.setSpeedMoveBehavoiur(500);
  //  rightClocker.executeMoveBehaviour();


  
  //leftClock.loop();
  //rightClock.loop();

//   if (ET_Ic2.receiveData())
// {
    
//   Serial.println(mydata.leftRacketSpeed);
//   if (mydata.rightTableHit == 1)
//     Serial.println("HitRighTable ");

//   if (mydata.leftTableHit == 1)
//     Serial.println("HitLefttTable ");

//   if (mydata.rightRacketHit == 1)
//   {
//     static int count = 0;
//     Serial.print("HitRightRacket : ");
//     Serial.println(count++);
//   }

//   if (mydata.leftRacketHit == 1)
//   {
//     static int count = 0;
//     Serial.print("HitLeftRacket : ");
//     Serial.println(count++);
//   }
// }

  const uint32_t now = millis();
  if (ET_Motor.receiveData()
      && pingpong::isCompatible(worldFrame)
      && (!hasValidFrame || worldFrame.frameSequence != lastFrameSequence)) {
#ifdef PINGPONG_LINK_DEBUG
    const bool leftRacketHit = hasValidFrame && worldFrame.leftRacketHitCount != lastLeftRacketHitCount;
    const bool rightRacketHit = hasValidFrame && worldFrame.rightRacketHitCount != lastRightRacketHitCount;
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

    if (pingpong::isRunning(worldFrame)) {
      const float roll = constrain(worldFrame.leftRacketRoll, -180.0F, 180.0F);
      currentRoll = static_cast<int>((roll + 180.0F) * maximumMotorSpeed / 360.0F);
      currentRoll = constrain(currentRoll, 0, maximumMotorSpeed);
    }

#ifdef PINGPONG_LINK_DEBUG
    if (rightTableHit) Serial.println("HitRightTable");
    if (leftTableHit) Serial.println("HitLeftTable");
    if (rightRacketHit) Serial.println("HitRightRacket");
    if (leftRacketHit) Serial.println("HitLeftRacket");
#endif
  }

  const bool communicationTimedOut = !hasValidFrame || now - lastValidFrameMs >= communicationTimeoutMs;
  if (communicationTimedOut || !pingpong::isRunning(worldFrame)) {
    currentRoll = 0;
    hasValidFrame = false;
  }

  motorClock.setSpeedMoveBehavoiur(currentRoll);
  motorClock.executeMoveBehaviour();
}

void receive(int numBytes) {}
void clientClocker(){}
