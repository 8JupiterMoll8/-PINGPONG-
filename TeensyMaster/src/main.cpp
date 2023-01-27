  #include "PingPong.h"


/*
███████╗ █████╗ ███████╗██╗   ██╗████████╗██████╗  █████╗ ███╗   ██╗███████╗███████╗███████╗██████╗ 
██╔════╝██╔══██╗██╔════╝╚██╗ ██╔╝╚══██╔══╝██╔══██╗██╔══██╗████╗  ██║██╔════╝██╔════╝██╔════╝██╔══██╗
█████╗  ███████║███████╗ ╚████╔╝    ██║   ██████╔╝███████║██╔██╗ ██║███████╗█████╗  █████╗  ██████╔╝
██╔══╝  ██╔══██║╚════██║  ╚██╔╝     ██║   ██╔══██╗██╔══██║██║╚██╗██║╚════██║██╔══╝  ██╔══╝  ██╔══██╗
███████╗██║  ██║███████║   ██║      ██║   ██║  ██║██║  ██║██║ ╚████║███████║██║     ███████╗██║  ██║
╚══════╝╚═╝  ╚═╝╚══════╝   ╚═╝      ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝╚═╝     ╚══════╝╚═╝  ╚═╝*/
EasyTransfer ET;
ETSENDATA mydata;


/*
██╗     ██╗ ██████╗ ██╗  ██╗████████╗
██║     ██║██╔════╝ ██║  ██║╚══██╔══╝
██║     ██║██║  ███╗███████║   ██║
██║     ██║██║   ██║██╔══██║   ██║
███████╗██║╚██████╔╝██║  ██║   ██║
╚══════╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝*/
// How many leds in your strip?
//const int NUM_LEDS = 360;
const int NUM_LEDS = 58;

CRGB LedStrip[NUM_LEDS];

void blinkAll(void);

void fadeall(void);

void cylon(void);

/*
██╗     ███████╗███████╗████████╗    ██████╗  █████╗  ██████╗██╗  ██╗███████╗████████╗
██║     ██╔════╝██╔════╝╚══██╔══╝    ██╔══██╗██╔══██╗██╔════╝██║ ██╔╝██╔════╝╚══██╔══╝
██║     █████╗  █████╗     ██║       ██████╔╝███████║██║     █████╔╝ █████╗     ██║
██║     ██╔══╝  ██╔══╝     ██║       ██╔══██╗██╔══██║██║     ██╔═██╗ ██╔══╝     ██║
███████╗███████╗██║        ██║       ██║  ██║██║  ██║╚██████╗██║  ██╗███████╗   ██║
╚══════╝╚══════╝╚═╝        ╚═╝       ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚══════╝   ╚═╝*/


// RF2- Reciver
const uint64_t lr_ADRESS{0xF0F0F0F0D2LL};//0xF0F0F0F0E1LL 0xF0F0F0F0D2LL
const byte lr_CHANNEL {121};             // 121
const byte lr_CE_PIN  {31};              // 31
const byte lr_CSN_PIN {32};              // 32

RF24 lr_radio(lr_CE_PIN, lr_CSN_PIN);
ReciverData lr_rf24SensorData;
Reciver lr_RF24_Reciver(lr_radio, lr_ADRESS, lr_CHANNEL, lr_rf24SensorData);

// HIT Behaviour
const int LR_PIEZO_THERSHOLD_MIN     {5};
const int LR_PIEZO_PEAKTRACK_MILLIS  {3};
const int LR_PIEZO_AFTERSCHOCK_MILLIS{25};

PeakDetector lr_PiezoDetector(LR_PIEZO_THERSHOLD_MIN, LR_PIEZO_PEAKTRACK_MILLIS, LR_PIEZO_AFTERSCHOCK_MILLIS);
Counter lr_PiezoCounter;
InputSensorRaw lr_PiezoInput(lr_rf24SensorData);
Piezo lr_Piezo(lr_PiezoDetector, lr_PiezoCounter, lr_PiezoInput);

// Motion Behaviour
Speed lr_speed(lr_rf24SensorData);
Swing lr_Swing(lr_rf24SensorData);
SF lr_fusion;
Mahony lr_Mahony(lr_rf24SensorData, lr_fusion);

// Button Behaviour
Pressure lr_pressure(lr_rf24SensorData);

// Left Racket
Racket leftRacket(lr_Piezo, lr_speed, lr_Swing, lr_Mahony, lr_pressure);

AudioVisualizer lr_Midi(leftRacket);

// AudioVisual Behaviour for Swing without Ballcontact
Bargraph lr_bargraph(LedStrip);

// AudioVisual Behaviour for Swing after Ballcontact
CometRaw lr_cometRaw(LedStrip);


/*
██████╗ ██╗ ██████╗ ██╗  ██╗████████╗    ██████╗  █████╗  ██████╗██╗  ██╗███████╗████████╗
██╔══██╗██║██╔════╝ ██║  ██║╚══██╔══╝    ██╔══██╗██╔══██╗██╔════╝██║ ██╔╝██╔════╝╚══██╔══╝
██████╔╝██║██║  ███╗███████║   ██║       ██████╔╝███████║██║     █████╔╝ █████╗     ██║
██╔══██╗██║██║   ██║██╔══██║   ██║       ██╔══██╗██╔══██║██║     ██╔═██╗ ██╔══╝     ██║
██║  ██║██║╚██████╔╝██║  ██║   ██║       ██║  ██║██║  ██║╚██████╗██║  ██╗███████╗   ██║
╚═╝  ╚═╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝       ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚══════╝   ╚═╝*/
// RF24 RECIVER
const uint64_t rr_ADRESS{0xF0F0F0F0E1LL};
const byte rr_CHANNEL{125};
const byte rr_CE_PIN {37};
const byte rr_CSN_PIN{38};

RF24 rr_radio(rr_CE_PIN, rr_CSN_PIN);
ReciverData rr_rf24SensorData;
Reciver rr_RF24_Reciver(rr_radio, rr_ADRESS, rr_CHANNEL, rr_rf24SensorData);
// HIT Behaviour
const int RR_PIEZO_THERSHOLD_MIN{5};
const int RR_PIEZO_PEAKTRACK_MILLIS{3};
const int RR_PIEZO_AFTERSCHOCK_MILLIS{25};

PeakDetector rr_PiezoDetector(RR_PIEZO_THERSHOLD_MIN, RR_PIEZO_PEAKTRACK_MILLIS, RR_PIEZO_AFTERSCHOCK_MILLIS);
Counter rr_PiezoCounter;
InputSensorRaw rr_PiezoInput(rr_rf24SensorData);
Piezo rr_Piezo(rr_PiezoDetector, rr_PiezoCounter, rr_PiezoInput);

// Motion Behaviour
Speed rr_speed(rr_rf24SensorData);
Swing rr_Swing(rr_rf24SensorData);
SF rr_fusion;
Mahony rr_Mahony(rr_rf24SensorData, rr_fusion);

// Button Behaviour
Pressure rr_pressure(rr_rf24SensorData);

// Left Racket
Racket rightRacket(rr_Piezo, rr_speed, rr_Swing, rr_Mahony, rr_pressure);

// AudioVisual Behaviour for Swing without Ballcontact
Bargraph rr_bargraph(LedStrip);
// AudioVisual Behaviour for Swing after Ballcontact
CometRaw rr_cometRaw(LedStrip);


/*
██╗     ███████╗███████╗████████╗    ████████╗ █████╗ ██████╗ ██╗     ███████╗
██║     ██╔════╝██╔════╝╚══██╔══╝    ╚══██╔══╝██╔══██╗██╔══██╗██║     ██╔════╝
██║     █████╗  █████╗     ██║          ██║   ███████║██████╔╝██║     █████╗
██║     ██╔══╝  ██╔══╝     ██║          ██║   ██╔══██║██╔══██╗██║     ██╔══╝
███████╗███████╗██║        ██║          ██║   ██║  ██║██████╔╝███████╗███████╗
╚══════╝╚══════╝╚═╝        ╚═╝          ╚═╝   ╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝
 */


/*PIEZO*/
const int LT_PIEZO_PIN{A15};
const int LT_PIEZO_THERSHOLD_MIN{30};
const int LT_PIEZO_PEAKTRACK_MILLIS{12};
const int LT_PIEZO_AFTERSCHOCK_MILLIS{20};

PeakDetector lt_PiezoDetector(LT_PIEZO_THERSHOLD_MIN, LT_PIEZO_PEAKTRACK_MILLIS, LT_PIEZO_AFTERSCHOCK_MILLIS);
ResponsiveAnalogRead lt_PiezoSmoother(LT_PIEZO_PIN, false);
InputSensorSmooth lt_PiezoInput(lt_PiezoSmoother);
Counter lt_PiezoCounter;
Piezo lt_Piezo(lt_PiezoDetector, lt_PiezoCounter, lt_PiezoInput);
Table leftTable(lt_Piezo);


/*
███╗   ██╗███████╗████████╗
████╗  ██║██╔════╝╚══██╔══╝
██╔██╗ ██║█████╗     ██║   
██║╚██╗██║██╔══╝     ██║   
██║ ╚████║███████╗   ██║   
╚═╝  ╚═══╝╚══════╝   ╚═╝   
*/                    
const int NET_PIEZO_PIN{A16};


/*
██████╗ ██╗ ██████╗ ██╗  ██╗████████╗    ████████╗ █████╗ ██████╗ ██╗     ███████╗
██╔══██╗██║██╔════╝ ██║  ██║╚══██╔══╝    ╚══██╔══╝██╔══██╗██╔══██╗██║     ██╔════╝
██████╔╝██║██║  ███╗███████║   ██║          ██║   ███████║██████╔╝██║     █████╗
██╔══██╗██║██║   ██║██╔══██║   ██║          ██║   ██╔══██║██╔══██╗██║     ██╔══╝
██║  ██║██║╚██████╔╝██║  ██║   ██║          ██║   ██║  ██║██████╔╝███████╗███████╗
╚═╝  ╚═╝╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝          ╚═╝   ╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝
 */

/*PIEZO*/
const int RT_PIEZO_PIN{A17};
const int RT_PIEZO_THERSHOLD_MIN{10};
const int RT_PIEZO_PEAKTRACK_MILLIS{3};
const int RT_PIEZO_AFTERSCHOCK_MILLIS{15}; //! This has to be fixed

PeakDetector rt_PiezoDetector(RT_PIEZO_THERSHOLD_MIN, RT_PIEZO_PEAKTRACK_MILLIS, RT_PIEZO_AFTERSCHOCK_MILLIS);
ResponsiveAnalogRead rt_PiezoSmoother(RT_PIEZO_PIN, false);
InputSensorSmooth rt_PiezoInput(rt_PiezoSmoother);
Counter rt_PiezoCounter;
Piezo rt_Piezo(rt_PiezoDetector, rt_PiezoCounter, rt_PiezoInput); // Composition
Table rightTable(rt_Piezo);


/*
 ██████╗  █████╗ ███╗   ███╗███████╗    ███╗   ███╗ █████╗ ███╗   ██╗ █████╗  ██████╗ ███████╗██████╗
██╔════╝ ██╔══██╗████╗ ████║██╔════╝    ████╗ ████║██╔══██╗████╗  ██║██╔══██╗██╔════╝ ██╔════╝██╔══██╗
██║  ███╗███████║██╔████╔██║█████╗      ██╔████╔██║███████║██╔██╗ ██║███████║██║  ███╗█████╗  ██████╔╝
██║   ██║██╔══██║██║╚██╔╝██║██╔══╝      ██║╚██╔╝██║██╔══██║██║╚██╗██║██╔══██║██║   ██║██╔══╝  ██╔══██╗
╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗    ██║ ╚═╝ ██║██║  ██║██║ ╚████║██║  ██║╚██████╔╝███████╗██║  ██║
 ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝    ╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝ ╚═════╝ ╚══════╝╚═╝  ╚═╝

*/

CollisionDetector collisionDetector(lr_cometRaw, rr_cometRaw);


/*
 ██████╗ ██████╗ ███╗   ███╗███╗   ███╗ █████╗ ███╗   ██╗██████╗
██╔════╝██╔═══██╗████╗ ████║████╗ ████║██╔══██╗████╗  ██║██╔══██╗
██║     ██║   ██║██╔████╔██║██╔████╔██║███████║██╔██╗ ██║██║  ██║
██║     ██║   ██║██║╚██╔╝██║██║╚██╔╝██║██╔══██║██║╚██╗██║██║  ██║
╚██████╗╚██████╔╝██║ ╚═╝ ██║██║ ╚═╝ ██║██║  ██║██║ ╚████║██████╔╝
 ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═════╝

*/

//Input input (rightRacket,leftRacket,rightTable,leftTable);
InputHandler inputHandler(rightRacket, leftRacket, rightTable, leftTable, rr_cometRaw, lr_cometRaw, LedStrip);

void peakHit(const Table &table, Racket &racket_, CRGB ledStrip[360]);

/*
███████╗████████╗ █████╗ ████████╗███████╗███╗   ███╗ █████╗  ██████╗██╗  ██╗██╗███╗   ██╗███████╗
██╔════╝╚══██╔══╝██╔══██╗╚══██╔══╝██╔════╝████╗ ████║██╔══██╗██╔════╝██║  ██║██║████╗  ██║██╔════╝
███████╗   ██║   ███████║   ██║   █████╗  ██╔████╔██║███████║██║     ███████║██║██╔██╗ ██║█████╗  
╚════██║   ██║   ██╔══██║   ██║   ██╔══╝  ██║╚██╔╝██║██╔══██║██║     ██╔══██║██║██║╚██╗██║██╔══╝  
███████║   ██║   ██║  ██║   ██║   ███████╗██║ ╚═╝ ██║██║  ██║╚██████╗██║  ██║██║██║ ╚████║███████╗
╚══════╝   ╚═╝   ╚═╝  ╚═╝   ╚═╝   ╚══════╝╚═╝     ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝╚══════╝
                                                                                                  
*/

BallWechselCounter ballwechselCounter(rightRacket, leftRacket, leftTable, rightTable);


void setup() {
    // Init Serial
    Serial.begin(115200);
    Serial1.begin(3000000);
   // while (!Serial) {}
    Serial.println("Hallo Ping Pong");

  // **Int Easy Transfer
    ET.begin(details(mydata), &Serial1);
    Wire2.begin();
  
   
    // **Init WS2182B
    byte dataPin = 26;
    byte clockPin = 27;

    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    LEDS.addLeds<SK9822, 26, 27, RGB, DATA_RATE_MHZ(10)>(LedStrip, NUM_LEDS);  // BGR ordering is typical
    LEDS.setBrightness(25);


    // Init RF24 Reciver
    pinMode(lr_CE_PIN, OUTPUT);
    pinMode(lr_CSN_PIN, OUTPUT);
    pinMode(rr_CE_PIN, OUTPUT);
    pinMode(rr_CSN_PIN, OUTPUT);

    digitalWrite(lr_CSN_PIN, HIGH);
    digitalWrite(rr_CSN_PIN, LOW);
    rr_RF24_Reciver.setup();
    delay(100);//TODO STRANGE THIS HAPPENS IF DELAYTIME IS SET WRONG  LEFt RAcket is not Recognized
    digitalWrite(rr_CSN_PIN, HIGH);
    digitalWrite(lr_CSN_PIN, LOW);
    lr_RF24_Reciver.setup();



    // Init Comet
    lr_cometRaw.setStartPosition(359.0);
    lr_cometRaw.reverseDirection();
    rr_cometRaw.setStartPosition(0.0);

for (int i = 0; i < 58; i++)
{
  LedStrip[i] = CRGB::Blue;
  FastLED.show();
  delay(10);
  // Now turn the LED off, then pause
  LedStrip[i] = CRGB::Black;
  FastLED.show();
  delay(10);
}


    // Shut OFF LED STRIP
   // FastLED.clear();

}

/*
██╗      ██████╗  ██████╗ ██████╗  ██╗██╗
██║     ██╔═══██╗██╔═══██╗██╔══██╗██╔╝╚██╗
██║     ██║   ██║██║   ██║██████╔╝██║  ██║
██║     ██║   ██║██║   ██║██╔═══╝ ██║  ██║
███████╗╚██████╔╝╚██████╔╝██║     ╚██╗██╔╝
╚══════╝ ╚═════╝  ╚═════╝ ╚═╝      ╚═╝╚═╝
 */
void loop() {
//delay(1);      // !! FOR DEBUGGING SERIAL PLotter

//LEFT RACKET
   digitalWrite(rr_CSN_PIN, HIGH); // turn OFF rr_RF24_Reciver
   digitalWrite(lr_CSN_PIN, LOW); // turn on lr_RF24_Reciver
   lr_RF24_Reciver.loop();
   leftRacket.loop();
//RIGHT RACKET
  digitalWrite(lr_CSN_PIN, HIGH); // turn OFF lr_RF24_Reciver
  digitalWrite(rr_CSN_PIN, LOW); 
  rr_RF24_Reciver.loop();
  rightRacket.loop();

 //LEft TABLE 
  rightTable.loop();

//RIGHT TABLE
  leftTable.loop();

//COMMANDS 
 inputHandler.loop();


FastLED.show();


//
// GAME_MANEGER_FOR_AUFSCHLAG_UND_BALLWECHSEL
   // ballwechselCounter.loop();
   // ballwechselCounter.getTotalBallwechsel();
   // ballwechselCounter.printDebug();

// Collison Detector 
//collisionDetector.isCollision();

// EASY_TRANSFER SEND DATA TO THE LIGHT BULB System
    mydata.leftRacketHit = leftRacket.isHit();
    mydata.leftRacketSpeed = leftRacket.roll();
    mydata.leftTableHit = leftTable.isHit();

    mydata.rightRacketHit = rightRacket.isHit();
    mydata.rightRacketSpeed = rightRacket.speed();
    mydata.rightTableHit = rightTable.isHit();


    ET.sendData();
  

// End Loop
}








