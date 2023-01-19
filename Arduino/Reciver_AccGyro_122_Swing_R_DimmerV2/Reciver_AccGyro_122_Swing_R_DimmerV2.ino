  // RECIVER: TEENSY 4.1 @ CHANEL 121

// Neopixels
#include <WS2812Serial.h>
#define USE_WS2812SERIAL
#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS  144
#define NUM_LEDS_2 144

//#define DATA_PIN 4
//#define DATA_PIN_2 5

// Define the array of leds
//CRGB leds   [NUM_LEDS];
//CRGB leds_2[NUM_LEDS_2];

// DIMMER 

#include <TimerOne.h>
 elapsedMillis ms;
int brightness = 5;    // how bright the LED is
int fadeAmount = 1;    // how many points to fade the LED by


void zero_crosss_int();
void timerIsr();
unsigned char channel_1 = 3;  // Output to Opto Triac pin, channel 1
unsigned char channel_2 = 4;  // Output to Opto Triac pin, channel 2
unsigned char channel_3 = 5;  // Output to Opto Triac pin, channel 3
unsigned char channel_4 = 6;  // Output to Opto Triac pin, channel 4
unsigned char channel_5 = 7;  // Output to Opto Triac pin, channel 5
unsigned char channel_6 = 8;  // Output to Opto Triac pin, channel 6
unsigned char channel_7 = 9; // Output to Opto Triac pin, channel 7
unsigned char channel_8 = 10; // Output to Opto Triac pin, channel 8
unsigned char CH1, CH2, CH3, CH4, CH5, CH6, CH7, CH8;
unsigned char CHANNEL_SELECT;
unsigned char i=0;
unsigned char clock_tick; // variable for Timer1
unsigned int delay_time = 150;

unsigned char low = 70;
unsigned char high = 5;


unsigned char CH[]={CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8};


//Mahony Algorytmus For Roll , Pitch Yaw
#include "SensorFusion.h"
SF fusion;

//NRF24
#include <SPI.h>
#include "printf.h"
#include "RF24.h"
 RF24 radio(21, 20); //CE pin, CSN pin

//NRF24
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

// Midi
const int MIDI_CH         = 2;
const int CC_ROLL        = 11;
const int CC_PITCH       = 12;
const int CC_YAW         = 13;
const int CC_SWING_A     = 14;
const int CC_SWING_B     = 15;
const int CC_SWING_TOTAL = 17;

// Global Variables used by opher Mudul
float g_totalAccXYZ;  // Acceleometer
float g_totalVelXYZ;   // Gyroscope;
float g_highPassPiezo;// Piezzo





void setup(void)
{
  Serial.begin(115200);

  while (!Serial)
    delay(10);
 
 setupRf24(125);
 setup_Dimmer();

   //LEDS.addLeds<WS2812SERIAL,DATA_PIN,  RGB>(leds,    NUM_LEDS);
  //LEDS.addLeds<WS2812SERIAL,DATA_PIN_2,RGB>(leds_2,NUM_LEDS_2);
  //LEDS.setBrightness(255);


}

void loop()
{

//delay(10);


loopRf24();

  
      // DrawComet();
       // leds[3].setHue(100);
       // leds_2[3].setHue(200);
      //FastLED.show();
//CH1=CH2=CH3=CH4=CH5=CH6=CH7=CH8=random(60);
//delay(random(50));




      
}
