#include <Arduino.h>
#include "AccelStepper.h"
#include "LeftClock.h"
#include "Clocker.h"
#include "IMoveBehaviour.h"
#include "MoveTickTack.h"
#include "MoveRandomly.h"
#include "MoveConstant.h"

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

EasyTransfer ET;
struct ET_ReciverData
{
 
   uint8_t  rightRacketHit;
   uint8_t  leftRacketHit;
   uint8_t  leftTableHit;
   uint8_t  rightTableHit;
   float leftRacketSpeed;
   float rightRacketSpeed;
};

//give a name to the group of data
ET_ReciverData mydata;
EasyTransferI2C ET_Ic2; 
void receive(int numBytes);


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
Clock clock(leftStepper);
MoveConstant moveConstant(leftStepper);
int currentRoll;


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
  Serial1.begin(3000000);    // EasyTransfer
//   while(!Serial )
//    {  
//     Serial.println("TEENSY-MOTOR");
//  }


    ET.begin(details(mydata), &Serial1);

    // Wire2.begin(9);
    //ET_Ic2.begin(details(mydata), &Wire2);   
    //Wire2.onReceive(receive);



 //leftClock.setup();
 //rightClock.setup();
 
 clock.setMoveBehaviour(&moveConstant);
 clock.setupMoveBehaviour();




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

  clock.setSpeedMoveBehavoiur(currentRoll);
  clock.executeMoveBehaviour();


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

if (ET.receiveData())
{   
  static int previousRoll = 0;
  currentRoll = map(mydata.leftRacketSpeed,-180.0, 180.0, 0, 9000);

 if (currentRoll != previousRoll) {
      //  Serial.println(currentRoll);
        previousRoll = currentRoll;
 }
 

  if (mydata.rightTableHit == 1) Serial.println("HitRighTable ");
  if (mydata.leftTableHit == 1)  Serial.println("HitLefttTable ");


  if (mydata.rightRacketHit == 1)
  {
    static int count = 0;
    Serial.print("HitRightRacket : ");
    Serial.println(count++);
  }


  if (mydata.leftRacketHit == 1)
  {
    static int count = 0;
    Serial.print("HitLeftRacket : ");
    Serial.println(count++);
  }


}
}

void receive(int numBytes) {}
void clientClocker(){}