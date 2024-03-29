#include <Arduino.h>
#//include "TimerOne.h"
//#include "Birnen.hpp"
//#include "KnightRider.h"
//#include "FadeAll.h"

#include <EasyTransfer.h>

/*
███████╗ █████╗ ███████╗██╗   ██╗████████╗██████╗  █████╗ ███╗   ██╗███████╗███████╗███████╗██████╗ 
██╔════╝██╔══██╗██╔════╝╚██╗ ██╔╝╚══██╔══╝██╔══██╗██╔══██╗████╗  ██║██╔════╝██╔════╝██╔════╝██╔══██╗
█████╗  ███████║███████╗ ╚████╔╝    ██║   ██████╔╝███████║██╔██╗ ██║███████╗█████╗  █████╗  ██████╔╝
██╔══╝  ██╔══██║╚════██║  ╚██╔╝     ██║   ██╔══██╗██╔══██║██║╚██╗██║╚════██║██╔══╝  ██╔══╝  ██╔══██╗
███████╗██║  ██║███████║   ██║      ██║   ██║  ██║██║  ██║██║ ╚████║███████║██║     ███████╗██║  ██║
╚══════╝╚═╝  ╚═╝╚══════╝   ╚═╝      ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝╚═╝     ╚══════╝╚═╝  ╚═╝
                                                                                                    
*/

EasyTransfer ET_Light;
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

int currentRoll;


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
  while(!Serial)
  {

  }
  ET_Light.begin(details(mydata), &Serial8);

  // INIT AC BULBS 240V PHASE CONTROLLER
  //setup_Dimmer();
  Serial.println("AC DIMMER 2");
  
pinMode(22,OUTPUT);


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



if (ET_Light.receiveData())
{   
  static int previousRoll = 0;
  currentRoll = map(mydata.leftRacketSpeed,-180.0, 180.0, 0, 9000);

 if (currentRoll != previousRoll) {
     // Serial.println(currentRoll);
        previousRoll = currentRoll;
 }
 

  if (mydata.rightTableHit == 1) Serial.println("HitRighTable ");
  if (mydata.leftTableHit == 1)  Serial.println("HitLefttTable ");


  if (mydata.rightRacketHit == 1)
  {
    static int count = 0;
    Serial.print("HitRightRacket : ");
    Serial.println(count++);
   
     digitalWrite(22,HIGH);
     delay(10);
  }else{
    digitalWrite(22,LOW);
  }

  if (mydata.leftRacketHit == 1)
  {
    static int count = 0;
    Serial.print("HitLeftRacket : ");
    Serial.println(count++);
     
  }


}





}