#include <AccelStepper.h>
#include <elapsedMillis.h>

elapsedMillis ms;
int incrementSpeed = 0;
int increaseStepperSpeed();
AccelStepper Xaxis(1, 3,4); // pin 2 = step, pin 5 = direction
//AccelStepper Yaxis(1, 3, 6); // pin 2 = step, pin 5 = direction



int ar_Speed[4] = {500 , 1500, 50, 5000};
const byte enablePin = 28;

void setup()
{
  Serial.begin(9600);
   pinMode(enablePin, OUTPUT);
   digitalWrite(enablePin, HIGH);

   Xaxis.setMaxSpeed(12800);
   Xaxis.setSpeed(10000); // had to slow for my motor
   Xaxis.setAcceleration(100.0);

  // Yaxis.setMaxSpeed(12800);
  // Yaxis.setSpeed(10000); // had to slow for my motor
  // Yaxis.setAcceleration(100.0);


 

   
}

void loop()
{   
  
  
    //Xaxis.runToNewPosition(0);
    //Xaxis.runToNewPosition(500);

    //Yaxis.runToNewPosition(0);
    //Yaxis.runToNewPosition(500);
    
    



  
    //increaseStepperSpeed();
   randomStepperSpeed();
  
  
  Xaxis.setSpeed(incrementSpeed);
//  Yaxis.setSpeed(incrementSpeed);
  
  Xaxis.runSpeed();
//  Yaxis.runSpeed();

    
}

int randomStepperSpeed()
{
  if(ms >1000)
  {
    ms= 0;
  
   incrementSpeed = ar_Speed[random(0, 2)];
   Xaxis.setCurrentPosition(0);
  // Xaxis.runToNewPosition(0);
    Serial.println(incrementSpeed); 
    Serial.print("CurrentPosition;"); 
    Serial.println(Xaxis.currentPosition ());
    
  }
  
}
int increaseStepperSpeed()
{
  if(ms >1000)
  {
    ms= 0;
    ++incrementSpeed;
    Serial.println(incrementSpeed);    
    
  }
  
}
