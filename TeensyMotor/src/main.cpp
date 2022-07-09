#include <Arduino.h>
#include "AccelStepper.h"
#include "Clock.h"
#include "IMoveBehaviour.h"
#include "MoveConstant.h"
#include <EasyTransfer.h>

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
 ██████╗██╗      ██████╗  ██████╗██╗  ██╗
██╔════╝██║     ██╔═══██╗██╔════╝██║ ██╔╝
██║     ██║     ██║   ██║██║     █████╔╝ 
██║     ██║     ██║   ██║██║     ██╔═██╗ 
╚██████╗███████╗╚██████╔╝╚██████╗██║  ██╗
 ╚═════╝╚══════╝ ╚═════╝  ╚═════╝╚═╝  ╚═╝
                                         
*/
    const byte X_STEPPER_STEP_PIN  {3};
    const byte X_STEPPER_DIR_PIN   {4};

    const byte Y_STEPPER_STEP_PIN  {5};
    const byte Y_STEPPER_DIR_PIN   {6};

    const byte A_STEPPER_STEP_PIN  {7};
    const byte A_STEPPER_DIR_PIN   {8};

    AccelStepper l_zeiger (1, X_STEPPER_STEP_PIN, X_STEPPER_DIR_PIN);
    MoveConstant lz_moveConstant(l_zeiger);
    IMoveBehaviour *lz_MoveConstant = &lz_moveConstant;






 // Clock clock;




void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  while(!Serial)
  {

  }
  ET.begin(details(mydata), &Serial1);


  Serial.println("AC DIMMER 2");
  

  //lz_moveConstant.setup();

}

void loop() 
{



//lz_MoveConstant->loop();
//lz_MoveConstant->setSpeed(7000);
//Serial.println(mydata.leftRacketSpeed);
// STATE: Wait
if (ET.receiveData())
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