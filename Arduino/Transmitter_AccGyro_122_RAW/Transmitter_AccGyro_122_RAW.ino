// LED STRIP
// Adafruit ICM20649
#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20649.h>
#include <Adafruit_Sensor.h>
Adafruit_ICM20649 icm;

//NRF24
#include <SPI.h>
#include "printf.h"
#include "RF24.h"
RF24 radio(4, 5); // using pin 7 for the CE pin, and pin 8 for the CSN pin

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
 // Serial.begin(115200);
  //while (!Serial) delay(10);
 
  byte channel = 125;
  setup_RF24L01(channel);
  setup_Icm20649();
}

void loop()
{
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
  sensorData.pz  = analogRead(A1);
  sensorData.fsr = analogRead(A9);
  
  bool report =  radio.write(&sensorData, sizeof(SensorData));
}
