
// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>
// Modified by Richard Gemmell Oct 2019

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device connected to pins 18 and 19.
//
// Consider using the I2CDevice class instead of Wire to read a sensor.

// Created 29 March 2006

// This example code is in the public domain.

#include <Arduino.h>
#include <i2c_driver.h>
#include <i2c_driver_wire.h>

int led = LED_BUILTIN;

struct TransmitData
{
  float gx;
  float gz;
  float gy;
  float ax;
  float ay;
  float az;
  int16_t pz;
  int16_t fsr;  
};

TransmitData data;

void setup()
{
  pinMode(led, OUTPUT);
  Wire.begin();                         // join i2c bus

  Serial.begin(9600);                    // start serial for output
  // while(!Serial)
  // {

  // }
}

void loop()
{
  Serial.print("requesting ("); 
  Serial.print(sizeof data); 
  Serial.print(" bytes)... ");
  if (Wire.requestFrom(0x40, sizeof data))
  {
    Wire.readBytes((byte*) &data, sizeof data);
    Serial.println("done");

    Serial.println(data.gx);
    Serial.println(data.gy);
    Serial.println(data.gz);
    Serial.println(data.ax);
    Serial.println(data.ay);
    Serial.println(data.az);
    Serial.println(data.pz);
    Serial.println(data.fsr);
  } 
  else {
    Serial.println("could not connect");
  }
 // delay(500);
}
   

