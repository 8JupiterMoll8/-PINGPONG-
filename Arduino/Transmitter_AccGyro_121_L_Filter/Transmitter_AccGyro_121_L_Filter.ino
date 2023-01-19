

// Adafruit ICM20649
#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20649.h>
#include <Adafruit_Sensor.h>
Adafruit_ICM20649 icm;


//NRF24
#include <SPI.h>
#include "printf.h"
#include "RF24.h"
RF24 radio(4, 5);                            // using pin 4 for the CE pin, and pin 5 for the CSN pin
const uint64_t adress = 0xF0F0F0F0D2LL;  
byte channel = 121;

// global NRF24 Data Structure transmitted over Rf24
struct SensorData
{
  float gx;
  float gz;
  float gy;
  float ax;
  float ay;
  float az;
  int   pz;
  int   fsr;
};
SensorData sensorData;



void setup(void)
{
  //Serial.begin(115200);
 // while (!Serial) delay(10);
 

  setup_RF24L01(channel);
  setup_Icm20649();
  
}

void loop()
{

  //Performance Speed Test Start
  // unsigned long start_timer = micros();

  //Get a new normalized sensor event
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  icm.getEvent(&accel, &gyro, &temp);

  //Transmitt SensorData Structure over RF24
  sensorData.gx  = gyro.gyro.x;
  sensorData.gy  = gyro.gyro.y;
  sensorData.gz  = gyro.gyro.z;
  sensorData.ax  = accel.acceleration.x;
  sensorData.ay  = accel.acceleration.y;
  sensorData.az  = accel.acceleration.z;
  sensorData.fsr = analogRead(A9);
  


  //Set Delay for Debugging in Serial Plotter
  //delay(6);
  static float EMA_a = 0.3;      //initialization of EMA alpha
  static int   EMA_S = 0;        //initialization of EMA S
  static int   sensorValue = 0;

  //High Pass Filter for removing Bass
  sensorValue = analogRead(A1);
  EMA_S = (EMA_a * sensorValue) + ((1 - EMA_a) * EMA_S); //run the EMA
  sensorValue = sensorValue - EMA_S;
  sensorData.pz = constrain (sensorValue, 5,1024);
  //Serial.println(sensorData.pz);
 
  
  
  bool report =  radio.write(&sensorData, sizeof(SensorData));

}
