#include <Arduino.h>
 //#include <TimerOne.h>
 //#include "Birnen.hpp"
// #include "KnightRider.h"
// #include "FadeAll.h"

#include <EasyTransfer.h>

/*
███████╗ █████╗ ███████╗██╗   ██╗████████╗██████╗  █████╗ ███╗   ██╗███████╗███████╗███████╗██████╗ 
██╔════╝██╔══██╗██╔════╝╚██╗ ██╔╝╚══██╔══╝██╔══██╗██╔══██╗████╗  ██║██╔════╝██╔════╝██╔════╝██╔══██╗
█████╗  ███████║███████╗ ╚████╔╝    ██║   ██████╔╝███████║██╔██╗ ██║███████╗█████╗  █████╗  ██████╔╝
██╔══╝  ██╔══██║╚════██║  ╚██╔╝     ██║   ██╔══██╗██╔══██║██║╚██╗██║╚════██║██╔══╝  ██╔══╝  ██╔══██╗
███████╗██║  ██║███████║   ██║      ██║   ██║  ██║██║  ██║██║ ╚████║███████║██║     ███████╗██║  ██║
╚══════╝╚═╝  ╚═╝╚══════╝   ╚═╝      ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝╚═╝     ╚══════╝╚═╝  ╚═╝
                                                                                                    
*/

EasyTransfer ET_Motor;
struct ET_ReciverData
{
   uint32_t leftRacketSpeed;
   uint32_t rightRacketSpeed;
   uint8_t  rightRacketHit;
   uint8_t  leftRacketHit;
   uint8_t  leftTableHit;
   uint8_t  rightTableHit;
};

//give a name to the group of data
ET_ReciverData mydata;




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
  Serial.begin(115200);
  Serial8.begin(115200);
  while(!Serial)
  {

  }
  ET_Motor.begin(details(mydata), &Serial8);

  // INIT AC BULBS 240V PHASE CONTROLLER
  //setup_Dimmer();
  Serial.println("AC DIMMER 2");
  

  //lz_moveConstant.setup();

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



if (ET_Motor.receiveData())
{

  if (mydata.rightTableHit == 1)
    Serial.println("HitRighTable ");

  if (mydata.leftTableHit == 1)
    Serial.println("HitLefttTable ");

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