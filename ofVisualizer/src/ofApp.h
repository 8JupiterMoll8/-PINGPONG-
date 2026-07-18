#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include <atomic>
#include <vector>

struct RacketTelemetry {
    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;
    float ax = 0.0f, ay = 0.0f, az = 0.0f;
    float gx = 0.0f, gy = 0.0f, gz = 0.0f;
    int piezo = 0;
    int hitPeak = 0;
    int gripPressure = 0;
    float swingSpeed = 0.0f;
    int pps = 0;
    int ageMs = 0;
    bool isLive = false;
};

struct TableTelemetry {
    int leftHitCount = 0;
    int rightHitCount = 0;
    int netHitCount = 0;
    int leftPeak = 0;
    int rightPeak = 0;
    int netPeak = 0;
};

struct GameTelemetry {
    int phase = 0;
    int expectedInput = 0;
    int rallyCount = 0;
    int connectionLevel = 0;
};

struct LedPixel {
    ofColor color = ofColor::black;
    float brightness = 0.0f;
};

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void exit() override;

    void keyPressed(int key) override;
    void windowResized(int w, int h) override;

    // Audio synthesis callback (runs in real-time high priority audio thread)
    void audioOut(ofSoundBuffer & buffer) override;

    // Serial & Telemetry processing
    void connectSerial();
    void processSerialData();
    void parseTelemetryJson(const string& jsonStr);

    // Visual & Debugger Rendering
    void drawLedStrip(int x, int y, int w, int h);
    void drawOscilloscope(int x, int y, int w, int h, const std::vector<float>& data, float minValue, float maxValue, const ofColor& color, const string& title);
    void drawRacketModel(float roll, float pitch, float yaw, const ofColor& color, const string& label);
    void drawTelemetryDashboard();
    void updateLedSimulation();

private:
    // Serial connection
    ofSerial serial;
    string serialBuffer;
    bool isSerialConnected = false;
    string connectedDeviceName = "Disconnected";

    // Telemetry data
    RacketTelemetry leftRacket;
    RacketTelemetry rightRacket;
    TableTelemetry tableData;
    GameTelemetry gameData;
    unsigned long lastPacketTime = 0;
    int framesReceivedCount = 0;

    // Audio Engine
    ofSoundStream soundStream;
    int sampleRate = 48000;
    double leftPhase = 0.0;
    double rightPhase = 0.0;
    double tablePhase = 0.0;
    
    // Thread-safe atomic audio parameters manipulated by main thread, read by audio thread
    std::atomic<float> leftAudioFreq{220.0f};
    std::atomic<float> rightAudioFreq{330.0f};
    std::atomic<float> leftAudioAmp{0.0f};
    std::atomic<float> rightAudioAmp{0.0f};
    std::atomic<float> tableAudioFreq{150.0f};
    std::atomic<float> tableAudioAmp{0.0f};

    // Oscilloscope History Buffers (for real-time signal debugging)
    static constexpr size_t MAX_HISTORY = 300;
    std::vector<float> leftPiezoHistory;
    std::vector<float> rightPiezoHistory;
    std::vector<float> leftSwingHistory;
    std::vector<float> rightSwingHistory;
    std::vector<float> leftGripHistory;
    std::vector<float> rightGripHistory;

    // 58-LED Simulation (mirrors physical table strip)
    std::vector<LedPixel> ledStrip;
    int prevLeftHitCount = 0;
    int prevRightHitCount = 0;
    int prevNetHitCount = 0;

    // GUI
    ofxPanel gui;
    ofxLabel guiStatus;
    ofxLabel guiPPS;
    ofxLabel guiRally;
    ofxToggle audioEnabled;
    ofxToggle motionSynthToggle;
    ofxSlider<float> audioMasterVolume;
    ofxButton reconnectBtn;

    // 3D Camera
    ofEasyCam cam;

    // High-Resolution TrueType Fonts (for crystal clear vector typography)
    ofTrueTypeFont fontTitle;
    ofTrueTypeFont fontStats;
    ofTrueTypeFont fontSmall;
    ofTrueTypeFont fontOsc;
};
