

// Wire Slave Sender
// by Nicholas Zambetti <http://www.zambetti.com>
// Modified by Richard Gemmell Oct 2019

// Demonstrates use of the Wire library
// Sends data as an I2C/TWI slave device
// Refer to the "Wire Master Reader" example for use with this
// To use it, connect a master to the Teensy on pins 16 and 17.
//
// Consider using the I2CRegisterSlave class instead of Wire to
// create an I2C slave.

// Created 29 March 2006

// This example code is in the public domain.
#include <SPI.h>
#include "printf.h"
#include "RF24.h"
#include "Reciver.hpp"
#include "ReciverData.hpp"
#include <Arduino.h>
#include <i2c_driver.h>
#include <i2c_driver_wire.h>


struct TransData
{
  float gx;
  float gz;
  float gy;
  float ax;
  float ay;
  float az;
  int pz;
  int fsr;          
};

TransData data;

void requestEvent();





// RF24 RECIVER 
const uint64_t ADRESS {0xF0F0F0F0D2LL};
const byte CHANNEL    {121};
const byte CE_PIN     {21};
const byte CSN_PIN    {20};        

RF24 radio(CE_PIN, CSN_PIN);
ReciverData lr_rf24SensorData;
Reciver lr_reciver(radio, ADRESS, CHANNEL, lr_rf24SensorData);

void setup()
{
  pinMode(24, OUTPUT);
  Wire.begin(0x40);        // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  //Wire.setClock(1000000);



  
  Serial.begin(9600);
  // while(!Serial)
  // {

  // }
  Serial.println("Hallo ");
  digitalWrite(24, HIGH); 

    lr_reciver.setup();


}

void loop()
{

  lr_reciver.loop();
  //data.pz = lr_rf24SensorData.fsr;
 // Serial.println(lr_rf24SensorData.fsr);
  
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  Serial.print("sending ("); 
  Serial.print(sizeof data);
  Serial.println(" bytes)");

  Wire.write((byte *)&data, sizeof data);
}











