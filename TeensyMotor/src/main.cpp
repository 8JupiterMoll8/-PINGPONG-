#include <Arduino.h>
//#include "Clock.h"

#include <EasyTransfer.h>
#include <Wire.h>
#include <EasyTransferI2C.h>

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
                                   
 //Clock clock;

void setup() {
  Serial.begin(115200);
  Serial1.begin(3000000);    // EasyTransfer
//   while(!Serial )
//    {  
//     Serial.println("TEENSY-MOTOR");
//  }
  ET.begin(details(mydata), &Serial1);

    Wire2.begin(9);
    ET_Ic2.begin(details(mydata), &Wire2);   
    Wire2.onReceive(receive);




}

void loop() 
{

  if (ET_Ic2.receiveData())
{
    
  Serial.println(mydata.leftRacketSpeed);
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

// if (ET.receiveData())
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
}

void receive(int numBytes) {}