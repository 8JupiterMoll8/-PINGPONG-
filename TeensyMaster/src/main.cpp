  #include "PingPong.h"


/*
███████╗ █████╗ ███████╗██╗   ██╗████████╗██████╗  █████╗ ███╗   ██╗███████╗███████╗███████╗██████╗ 
██╔════╝██╔══██╗██╔════╝╚██╗ ██╔╝╚══██╔══╝██╔══██╗██╔══██╗████╗  ██║██╔════╝██╔════╝██╔════╝██╔══██╗
█████╗  ███████║███████╗ ╚████╔╝    ██║   ██████╔╝███████║██╔██╗ ██║███████╗█████╗  █████╗  ██████╔╝
██╔══╝  ██╔══██║╚════██║  ╚██╔╝     ██║   ██╔══██╗██╔══██║██║╚██╗██║╚════██║██╔══╝  ██╔══╝  ██╔══██╗
███████╗██║  ██║███████║   ██║      ██║   ██║  ██║██║  ██║██║ ╚████║███████║██║     ███████╗██║  ██║
╚══════╝╚═╝  ╚═╝╚══════╝   ╚═╝      ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝╚═╝     ╚══════╝╚═╝  ╚═╝*/
EasyTransfer ET_Motor;
EasyTransfer ET_Light;

pingpong::WorldFrame worldFrame{};

constexpr uint32_t outputFrameIntervalMs{10}; // 100 Hz for remote renderers
elapsedMillis outputFrameTimer;
constexpr uint32_t webTelemetryIntervalMs{20}; // 50 Hz — snappier webui response
constexpr int webTelemetryMinimumWriteBytes{1024}; // Drop diagnostics rather than wait on USB
constexpr uint32_t racketFreshTimeoutMs{250};
elapsedMillis webTelemetryTimer;
WebTelemetry webTelemetry{Serial};

void sendWorldFrame();
void sendWebTelemetry();
void fillRacketTelemetry(RacketTelemetry& telemetry, Racket& racket, const ReciverData& rawData);


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

// Dual Receiver Manager — keeps both NRF24 receivers listening continuously
DualReceiverManager dualReceiverManager(
    lr_RF24_Reciver, rr_RF24_Reciver,
    lr_CSN_PIN, rr_CSN_PIN // Dedicated chip-select pin for each receiver
); // Both radios share MOSI, MISO, and SCK


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
const int NT_PIEZO_THERSHOLD_MIN{20};
const int NT_PIEZO_PEAKTRACK_MILLIS{8};
const int NT_PIEZO_AFTERSCHOCK_MILLIS{20};

PeakDetector nt_PiezoDetector(NT_PIEZO_THERSHOLD_MIN, NT_PIEZO_PEAKTRACK_MILLIS, NT_PIEZO_AFTERSCHOCK_MILLIS);
ResponsiveAnalogRead nt_PiezoSmoother(NET_PIEZO_PIN, false);
InputSensorSmooth nt_PiezoInput(nt_PiezoSmoother);
Counter nt_PiezoCounter;
Piezo nt_Piezo(nt_PiezoDetector, nt_PiezoCounter, nt_PiezoInput);
Table netTable(nt_Piezo);


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
    Serial1.begin(6000000);
    Serial8.begin(6000000);

   // while (!Serial) {}
    Serial.println("Hallo Ping Pong");

  // **Int Easy Transfer
    ET_Motor.begin(details(worldFrame), &Serial1);
    ET_Light.begin(details(worldFrame), &Serial8);

  
  //** IC2 TRANSFER
   // Wire2.begin();
  
   
    // **Init WS2182B
    byte dataPin = 26;
    byte clockPin = 27;

    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    LEDS.addLeds<SK9822, 26, 27, RGB, DATA_RATE_MHZ(10)>(LedStrip, NUM_LEDS);  // BGR ordering is typical
    LEDS.setBrightness(25);


    // Initialize both RF24 receivers on their shared SPI bus.
    // Each radio stays in listening mode continuously.
    dualReceiverManager.setup(); // Reports a warning if either radio fails



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

    worldFrame.systemStatus = static_cast<uint8_t>(pingpong::SystemStatus::running);

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
#ifdef PINGPONG_RF24_STATS
elapsedMillis debugStatsTimer;
#endif

void loop() {
//delay(1);      // !! FOR DEBUGGING SERIAL PLotter

// RF24 RECEIVERS — poll both and retain each racket's newest sample
  dualReceiverManager.loop();
  leftRacket.loop();
  rightRacket.loop();

#ifdef PINGPONG_RF24_STATS
  if (debugStatsTimer >= 1000) {
    debugStatsTimer = 0;
    dualReceiverManager.printStats();
    dualReceiverManager.resetStats();
  }
#endif

//LEFT TABLE
  leftTable.loop();

//RIGHT TABLE
  rightTable.loop();

//NET TABLE
  netTable.loop();

//COMMANDS 
 inputHandler.loop();


FastLED.show();


//
// GAME_MANEGER_FOR_AUFSCHLAG_UND_BALLWECHSEL
   ballwechselCounter.loop();
   ballwechselCounter.printDebug();

// Collison Detector 
//collisionDetector.isCollision();

    sendWorldFrame();
    sendWebTelemetry();
  

// End Loop
}

void sendWorldFrame()
{
    if (outputFrameTimer < outputFrameIntervalMs) {
        return;
    }

    outputFrameTimer = 0;
    ++worldFrame.frameSequence;
    worldFrame.leftRacketRoll = leftRacket.roll();
    worldFrame.rightRacketSpeed = rightRacket.speed();
    worldFrame.leftRacketHitCount = static_cast<uint8_t>(leftRacket.hitSum());
    worldFrame.rightRacketHitCount = static_cast<uint8_t>(rightRacket.hitSum());
    worldFrame.leftTableHitCount = static_cast<uint8_t>(leftTable.hitSum());
    worldFrame.rightTableHitCount = static_cast<uint8_t>(rightTable.hitSum());

    ET_Motor.sendData();
    ET_Light.sendData();
}

void sendWebTelemetry()
{
    if (webTelemetryTimer < webTelemetryIntervalMs
        || !Serial
        || Serial.availableForWrite() < webTelemetryMinimumWriteBytes) {
        return;
    }

    webTelemetryTimer = 0;

    WebTelemetryFrame telemetry{};
    telemetry.timestampMs = millis();
    telemetry.frameSequence = worldFrame.frameSequence;
    telemetry.systemStatus = worldFrame.systemStatus;

    telemetry.radio.leftPackets = dualReceiverManager.getLeftPackets();
    telemetry.radio.rightPackets = dualReceiverManager.getRightPackets();
    telemetry.radio.leftAgeMs = dualReceiverManager.getLeftPacketAgeMs();
    telemetry.radio.rightAgeMs = dualReceiverManager.getRightPacketAgeMs();
    telemetry.radio.leftFresh = dualReceiverManager.isLeftFresh(racketFreshTimeoutMs);
    telemetry.radio.rightFresh = dualReceiverManager.isRightFresh(racketFreshTimeoutMs);

    fillRacketTelemetry(telemetry.left, leftRacket, lr_rf24SensorData);
    fillRacketTelemetry(telemetry.right, rightRacket, rr_rf24SensorData);

    telemetry.table.leftHitCount = static_cast<uint8_t>(leftTable.hitSum());
    telemetry.table.rightHitCount = static_cast<uint8_t>(rightTable.hitSum());
    telemetry.table.netHitCount = static_cast<uint8_t>(netTable.hitSum());
    telemetry.table.leftHitPeak = leftTable.hitSum() > 0
        ? static_cast<uint8_t>(constrain(leftTable.hitPeak(), 0, 127))
        : 0;
    telemetry.table.rightHitPeak = rightTable.hitSum() > 0
        ? static_cast<uint8_t>(constrain(rightTable.hitPeak(), 0, 127))
        : 0;
    telemetry.table.netHitPeak = netTable.hitSum() > 0
        ? static_cast<uint8_t>(constrain(netTable.hitPeak(), 0, 127))
        : 0;

    telemetry.game.phase         = ballwechselCounter.getPhase();
    telemetry.game.expectedInput = ballwechselCounter.getExpectedInput();
    telemetry.game.rallyCount    = ballwechselCounter.getRallyCount();

    const float constrainedRoll = constrain(worldFrame.leftRacketRoll, -180.0F, 180.0F);
    telemetry.outputs.motorTarget = static_cast<int32_t>((constrainedRoll + 180.0F) * 9000.0F / 360.0F);
    telemetry.outputs.lightPulseCount = worldFrame.rightRacketHitCount;

    webTelemetry.send(telemetry);
}

void fillRacketTelemetry(RacketTelemetry& telemetry, Racket& racket, const ReciverData& rawData)
{
    telemetry.gx = rawData.gx;
    telemetry.gy = rawData.gy;
    telemetry.gz = rawData.gz;
    telemetry.ax = rawData.ax;
    telemetry.ay = rawData.ay;
    telemetry.az = rawData.az;
    telemetry.piezo = rawData.pz;
    telemetry.pressure = racket.pressure();
    telemetry.roll = racket.roll();
    telemetry.pitch = racket.pitch();
    telemetry.yaw = racket.yaw();
    telemetry.speed = racket.speed();

    const int hitCount = racket.hitSum();
    telemetry.hitCount = static_cast<uint8_t>(hitCount);
    telemetry.hitPeak = hitCount > 0
        ? static_cast<uint8_t>(constrain(racket.hitPeak(), 0, 127))
        : 0;
}
