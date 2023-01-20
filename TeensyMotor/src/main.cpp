#include <Arduino.h>
#include "AccelStepper.h"
#include "LeftClock.h"
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

AccelStepper leftStepper(1,31,32);
//AccelStepper rightStepper(1,30,34);                              
//LeftClock leftClock(leftStepper);
//LeftClock rightClock(rightStepper);
Clock clock(leftStepper);

MoveConstant moveConstant(leftStepper);


elapsedMillis ms;
boolean toogleMoveBehaviour = false;

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
    ET_Ic2.begin(details(mydata), &Wire2);   
    //Wire2.onReceive(receive);




//leftClock.setup();
clock.setupMoveBehaviour();




}

void loop() 
{
  clock.setSpeedMoveBehavoiur(1000);
  clock.setMoveBehaviour(&moveConstant);
  clock.executeMoveBehaviour();
//leftClock.loop();


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
  //Serial.println(mydata.leftRacketSpeed);

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