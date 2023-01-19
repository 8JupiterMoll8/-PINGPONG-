#ifndef BIRNEN_H
#define BIRNEN_H
#pragma once
#include <TimerOne.h>
#include "Arduino.h"

elapsedMillis ms;

void zero_crosss_int();
void timerIsr();
const byte NUM_BULB = 8;

unsigned char channel[NUM_BULB] = {22,3,4,5,17,16,15,14};
unsigned char CH1, CH2, CH3, CH4, CH5, CH6, CH7, CH8;
unsigned char CH[8]={CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8};

unsigned char l_channel[NUM_BULB] = {40,39,38,37,36,35,34,33};
unsigned char l_CH1, l_CH2, l_CH3, l_CH4, l_CH5, l_CH6, l_CH7, l_CH8;
unsigned char l_CH[8]={ l_CH1, l_CH2, l_CH3, l_CH4, l_CH5, l_CH6, l_CH7, l_CH8};

unsigned char i=0;
volatile  unsigned char clock_tick; // variable for Timer1


unsigned char low = 70;
unsigned char high = 5;



void setup_Dimmer()
{

  for (int i = 0; i < NUM_BULB; i++) 
  {
    pinMode(channel[i], OUTPUT); 
    pinMode(l_channel[i], OUTPUT); // Set AC Load pin as output
  }

  pinMode(2, INPUT_PULLUP);
  attachInterrupt(2, zero_crosss_int, FALLING);
  Timer1.initialize(100); // set a timer of length 100 microseconds for 50Hz or 83 microseconds for 60Hz;
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
 


}

void timerIsr()
{
  clock_tick++;

  for (int i = 0; i < NUM_BULB; i++)
  {
    if (CH[i] == clock_tick)
    {
      digitalWrite(channel[i], HIGH); // triac firing
      delayMicroseconds(5);           // triac On propogation delay (for 60Hz use 8.33)
      digitalWrite(channel[i], LOW);  // triac Off
    }

    if (l_CH[i] == clock_tick)
    {
      digitalWrite(l_channel[i], HIGH); // triac firing
      delayMicroseconds(5);           // triac On propogation delay (for 60Hz use 8.33)
      digitalWrite(l_channel[i], LOW);  // triac Off
    }
  }

}


void zero_crosss_int() // function to be fired at the zero crossing to dim the light
{
  // Every zerocrossing interrupt: For 50Hz (1/2 Cycle) => 10ms ; For 60Hz (1/2 Cycle) => 8.33ms
  // 10ms=10000us , 8.33ms=8330us

  clock_tick = 0;
}
#endif