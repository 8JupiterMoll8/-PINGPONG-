

// DIMMER 

#include <TimerOne.h>
 elapsedMillis ms;
int brightness = 5;    // how bright the LED is
int fadeAmount = 1;    // how many points to fade the LED by


void zero_crosss_int();
void timerIsr();
// 22,3,4,5,17,16,15,14
unsigned char channel_1 = 22;  // Output to Opto Triac pin, channel 1
unsigned char channel_2 = 3;  // Output to Opto Triac pin, channel 2
unsigned char channel_3 = 4;  // Output to Opto Triac pin, channel 3
unsigned char channel_4 = 5;  // Output to Opto Triac pin, channel 4
unsigned char channel_5 = 17;  // Output to Opto Triac pin, channel 5
unsigned char channel_6 = 16;  // Output to Opto Triac pin, channel 6
unsigned char channel_7 = 15; // Output to Opto Triac pin, channel 7
unsigned char channel_8 = 14; // Output to Opto Triac pin, channel 8
unsigned char CH1, CH2, CH3, CH4, CH5, CH6, CH7, CH8;
unsigned char CHANNEL_SELECT;
unsigned char i=0;
unsigned char clock_tick; // variable for Timer1
unsigned int delay_time = 150;

unsigned char low = 70;
unsigned char high = 5;


unsigned char CH[]={CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8};











void setup(void)
{
  Serial.begin(115200);

 //while (!Serial);
  Serial.println("hallo dimmer");

 setup_Dimmer();



}

void loop()
{


//CH1=CH2=CH3=CH4=CH5=CH6=CH7=CH8=20;








      
}
