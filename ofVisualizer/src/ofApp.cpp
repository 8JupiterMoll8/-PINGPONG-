#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowTitle("Ping Pong AV-Debugger & Real-Time Sound Synthesizer");
    ofSetFrameRate(120); // 120 FPS native high-speed engine
    ofSetVerticalSync(true);
    ofBackground(15, 17, 22); // Deep dark background
    ofEnableAntiAliasing();

    // Initialize Oscilloscope buffers
    leftPiezoHistory.resize(MAX_HISTORY, 0.0f);
    rightPiezoHistory.resize(MAX_HISTORY, 0.0f);
    leftSwingHistory.resize(MAX_HISTORY, 0.0f);
    rightSwingHistory.resize(MAX_HISTORY, 0.0f);
    leftGripHistory.resize(MAX_HISTORY, 0.0f);
    rightGripHistory.resize(MAX_HISTORY, 0.0f);

    // Initialize 58-LED Top Strip
    ledStrip.resize(58);
    for (auto& pixel : ledStrip) {
        pixel.color = ofColor(20, 20, 25);
        pixel.brightness = 0.0f;
    }

    // Load high-resolution vector TrueType fonts from Pop!_OS system (Large & Bold sizes)
    fontTitle.load("/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf", 22, true, true);
    fontStats.load("/usr/share/fonts/truetype/ubuntu/UbuntuMono-B.ttf", 18, true, true);
    fontStats.setLineHeight(28.0f);
    fontSmall.load("/usr/share/fonts/truetype/ubuntu/UbuntuMono-B.ttf", 15, true, true);
    fontOsc.load("/usr/share/fonts/truetype/ubuntu/UbuntuMono-B.ttf", 14, true, true);

    // Setup GUI control panel with large clean vector font
    ofxGuiSetFont("/usr/share/fonts/truetype/ubuntu/UbuntuMono-B.ttf", 13, true, true);
    ofxGuiSetDefaultWidth(330);
    gui.setup("AV-Debugger Controls");
    gui.setPosition(20, 20);
    gui.add(guiStatus.setup("Serial Status", "Connecting..."));
    gui.add(guiPPS.setup("Rackets PPS", "L: 0 | R: 0"));
    gui.add(guiRally.setup("Game Rally", "0"));
    gui.add(audioEnabled.setup("Audio Synth Enabled", true));
    gui.add(motionSynthToggle.setup("Motion Pitch Modulation", true));
    gui.add(audioMasterVolume.setup("Master Volume", 0.5f, 0.0f, 1.0f));
    reconnectBtn.setup("Reconnect Serial");
    reconnectBtn.addListener(this, &ofApp::connectSerial);
    gui.add(&reconnectBtn);

    // Setup 3D camera
    cam.setDistance(550);
    cam.setPosition(0, 120, 550);
    cam.lookAt(ofVec3f(0, 0, 0));

    // Initialize Audio Engine (2 output channels stereo, 0 input channels, 48kHz, buffer size 1024)
    ofSoundStreamSettings settings;
    settings.setOutListener(this);
    settings.sampleRate = sampleRate;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.bufferSize = 1024;
    
    // Explicitly use PULSE or ALSA API on Linux to avoid PipeWire/JACK dynamic buffer size loops
    settings.setApi(ofSoundDevice::Api::PULSE);
    if (!soundStream.setup(settings)) {
        ofLogNotice("ofApp") << "PulseAudio setup failed, trying ALSA...";
        settings.setApi(ofSoundDevice::Api::ALSA);
        if (!soundStream.setup(settings)) {
            ofLogNotice("ofApp") << "ALSA setup failed, trying default API...";
            settings.setApi(ofSoundDevice::Api::UNSPECIFIED);
            soundStream.setup(settings);
        }
    }

    // Open serial port
    connectSerial();
}

void ofApp::connectSerial() {
    serial.close();
    isSerialConnected = false;
    connectedDeviceName = "Disconnected";
    guiStatus = "Disconnected";

    auto devices = serial.getDeviceList();
    for (auto& dev : devices) {
        if (dev.getDevicePath().find("ttyACM") != string::npos || dev.getDevicePath().find("ttyUSB") != string::npos) {
            ofLogNotice("ofApp") << "Found potential Teensy device: " << dev.getDevicePath();
            if (serial.setup(dev.getDevicePath(), 115200)) {
                isSerialConnected = true;
                connectedDeviceName = dev.getDevicePath();
                guiStatus = "Connected: " + connectedDeviceName;
                ofLogNotice("ofApp") << "Successfully opened " << connectedDeviceName;
                break;
            }
        }
    }

    if (!isSerialConnected && !devices.empty()) {
        if (serial.setup(0, 115200)) {
            isSerialConnected = true;
            connectedDeviceName = devices[0].getDevicePath();
            guiStatus = "Connected: " + connectedDeviceName;
        }
    }
}

void ofApp::update() {
    if (isSerialConnected) {
        processSerialData();
    }

    // Check freshness timestamps
    unsigned long now = ofGetElapsedTimeMillis();
    if (now - lastPacketTime > 500) {
        leftRacket.isLive = false;
        rightRacket.isLive = false;
    }

    // Update LED strip comet decay
    updateLedSimulation();

    // Decay audio envelope amplitudes continuously
    float decay = 0.94f;
    leftAudioAmp.store(leftAudioAmp.load() * decay);
    rightAudioAmp.store(rightAudioAmp.load() * decay);
    tableAudioAmp.store(tableAudioAmp.load() * (decay * 0.98f));

    guiPPS = "L: " + ofToString(leftRacket.pps) + " | R: " + ofToString(rightRacket.pps);
    guiRally = ofToString(gameData.rallyCount) + " (Phase: " + ofToString(gameData.phase) + ")";
}

void ofApp::updateLedSimulation() {
    for (auto& pixel : ledStrip) {
        pixel.brightness *= 0.91f;
        if (pixel.brightness < 0.02f) {
            pixel.brightness = 0.0f;
        }
    }
}

void ofApp::processSerialData() {
    while (serial.available() > 0) {
        unsigned char ch = serial.readByte();
        if (ch == '\n' || ch == '\r') {
            if (!serialBuffer.empty()) {
                if (serialBuffer.rfind("PP:", 0) == 0) {
                    string jsonStr = serialBuffer.substr(3);
                    parseTelemetryJson(jsonStr);
                }
                serialBuffer.clear();
            }
        } else {
            serialBuffer += (char)ch;
            if (serialBuffer.length() > 4096) {
                serialBuffer.clear();
            }
        }
    }
}

void ofApp::parseTelemetryJson(const string& jsonStr) {
    try {
        ofJson json = ofJson::parse(jsonStr);
        lastPacketTime = ofGetElapsedTimeMillis();
        framesReceivedCount++;

        // Radio Stats
        if (json.contains("radio")) {
            auto& radio = json["radio"];
            leftRacket.pps = radio.value("leftPackets", 0);
            rightRacket.pps = radio.value("rightPackets", 0);
            leftRacket.ageMs = radio.value("leftAge", 0);
            rightRacket.ageMs = radio.value("rightAge", 0);
            leftRacket.isLive = (radio.value("leftFresh", 0) == 1);
            rightRacket.isLive = (radio.value("rightFresh", 0) == 1);
        }

        // Left Racket
        if (json.contains("left")) {
            auto& l = json["left"];
            if (l.contains("orientation")) {
                leftRacket.roll = l["orientation"].value("roll", 0.0f);
                leftRacket.pitch = l["orientation"].value("pitch", 0.0f);
                leftRacket.yaw = l["orientation"].value("yaw", 0.0f);
            } else {
                leftRacket.roll = l.value("roll", 0.0f);
                leftRacket.pitch = l.value("pitch", 0.0f);
                leftRacket.yaw = l.value("yaw", 0.0f);
            }
            leftRacket.gx = l.value("gx", 0.0f);
            leftRacket.gy = l.value("gy", 0.0f);
            leftRacket.gz = l.value("gz", 0.0f);
            leftRacket.ax = l.value("ax", 0.0f);
            leftRacket.ay = l.value("ay", 0.0f);
            leftRacket.az = l.value("az", 0.0f);
            leftRacket.piezo = l.value("piezo", 0);
            leftRacket.hitPeak = l.contains("peak") ? l.value("peak", 0) : l.value("hitPeak", 0);
            leftRacket.gripPressure = l.contains("pressure") ? l.value("pressure", 0) : l.value("gripPressure", 0);
            leftRacket.swingSpeed = l.contains("speed") ? l.value("speed", 0.0f) : l.value("swingSpeed", 0.0f);

            // Push to Oscilloscope History vectors
            leftPiezoHistory.push_back((float)leftRacket.piezo);
            leftPiezoHistory.erase(leftPiezoHistory.begin());
            leftSwingHistory.push_back(leftRacket.swingSpeed);
            leftSwingHistory.erase(leftSwingHistory.begin());
            leftGripHistory.push_back((float)leftRacket.gripPressure);
            leftGripHistory.erase(leftGripHistory.begin());

            // Audio Synthesis Trigger & Modulation
            if (leftRacket.hitPeak > 5) {
                leftAudioAmp.store(std::min(1.0f, leftRacket.hitPeak / 40.0f));
            }
            if (motionSynthToggle) {
                float freq = 200.0f + (leftRacket.swingSpeed * 5.0f) + (leftRacket.gripPressure * 0.5f);
                leftAudioFreq.store(ofClamp(freq, 120.0f, 1200.0f));
            }
        }

        // Right Racket
        if (json.contains("right")) {
            auto& r = json["right"];
            if (r.contains("orientation")) {
                rightRacket.roll = r["orientation"].value("roll", 0.0f);
                rightRacket.pitch = r["orientation"].value("pitch", 0.0f);
                rightRacket.yaw = r["orientation"].value("yaw", 0.0f);
            } else {
                rightRacket.roll = r.value("roll", 0.0f);
                rightRacket.pitch = r.value("pitch", 0.0f);
                rightRacket.yaw = r.value("yaw", 0.0f);
            }
            rightRacket.gx = r.value("gx", 0.0f);
            rightRacket.gy = r.value("gy", 0.0f);
            rightRacket.gz = r.value("gz", 0.0f);
            rightRacket.ax = r.value("ax", 0.0f);
            rightRacket.ay = r.value("ay", 0.0f);
            rightRacket.az = r.value("az", 0.0f);
            rightRacket.piezo = r.value("piezo", 0);
            rightRacket.hitPeak = r.contains("peak") ? r.value("peak", 0) : r.value("hitPeak", 0);
            rightRacket.gripPressure = r.contains("pressure") ? r.value("pressure", 0) : r.value("gripPressure", 0);
            rightRacket.swingSpeed = r.contains("speed") ? r.value("speed", 0.0f) : r.value("swingSpeed", 0.0f);

            // Push to Oscilloscope History vectors
            rightPiezoHistory.push_back((float)rightRacket.piezo);
            rightPiezoHistory.erase(rightPiezoHistory.begin());
            rightSwingHistory.push_back(rightRacket.swingSpeed);
            rightSwingHistory.erase(rightSwingHistory.begin());
            rightGripHistory.push_back((float)rightRacket.gripPressure);
            rightGripHistory.erase(rightGripHistory.begin());

            // Audio Synthesis Trigger & Modulation
            if (rightRacket.hitPeak > 5) {
                rightAudioAmp.store(std::min(1.0f, rightRacket.hitPeak / 40.0f));
            }
            if (motionSynthToggle) {
                float freq = 320.0f + (rightRacket.swingSpeed * 6.0f) + (rightRacket.gripPressure * 0.5f);
                rightAudioFreq.store(ofClamp(freq, 180.0f, 1400.0f));
            }
        }

        // Table
        if (json.contains("table")) {
            auto& t = json["table"];
            tableData.leftHitCount = t.value("leftHit", 0);
            tableData.rightHitCount = t.value("rightHit", 0);
            tableData.netHitCount = t.value("netHit", 0);
            tableData.leftPeak = t.value("leftPeak", 0);
            tableData.rightPeak = t.value("rightPeak", 0);
            tableData.netPeak = t.value("netPeak", 0);

            // Trigger LED Comets & Table Audio on hit changes
            if (tableData.leftHitCount > prevLeftHitCount) {
                prevLeftHitCount = tableData.leftHitCount;
                tableAudioFreq.store(160.0f); // Deep resonant ping
                tableAudioAmp.store(0.9f);
                for (int i = 40; i < 58; i++) {
                    ledStrip[i].color = ofColor(255, 80, 120); // Pink/Red left hit
                    ledStrip[i].brightness = 1.0f;
                }
            }
            if (tableData.rightHitCount > prevRightHitCount) {
                prevRightHitCount = tableData.rightHitCount;
                tableAudioFreq.store(240.0f); // High resonant pong
                tableAudioAmp.store(0.9f);
                for (int i = 0; i < 18; i++) {
                    ledStrip[i].color = ofColor(80, 240, 210); // Teal right hit
                    ledStrip[i].brightness = 1.0f;
                }
            }
            if (tableData.netHitCount > prevNetHitCount) {
                prevNetHitCount = tableData.netHitCount;
                tableAudioFreq.store(80.0f); // Net thump
                tableAudioAmp.store(1.0f);
                for (int i = 25; i < 33; i++) {
                    ledStrip[i].color = ofColor(255, 255, 255); // White net flash
                    ledStrip[i].brightness = 1.0f;
                }
            }
        }

        // Game
        if (json.contains("game")) {
            auto& g = json["game"];
            gameData.phase = g.value("phase", 0);
            gameData.expectedInput = g.value("expected", 0);
            gameData.rallyCount = g.value("rally", 0);
            gameData.connectionLevel = g.value("level", 0);
        }
    } catch (const std::exception& e) {
        ofLogError("ofApp") << "JSON parse error: " << e.what();
    }
}

// Real-time Audio Callback (Stereo synthesis)
void ofApp::audioOut(ofSoundBuffer & buffer) {
    if (!audioEnabled) {
        buffer.set(0.0f);
        return;
    }

    float masterVol = audioMasterVolume;
    float lFreq = leftAudioFreq.load();
    float rFreq = rightAudioFreq.load();
    float lAmp = leftAudioAmp.load();
    float rAmp = rightAudioAmp.load();
    float tFreq = tableAudioFreq.load();
    float tAmp = tableAudioAmp.load();

    double lStep = (lFreq * TWO_PI) / (double)sampleRate;
    double rStep = (rFreq * TWO_PI) / (double)sampleRate;
    double tStep = (tFreq * TWO_PI) / (double)sampleRate;

    for (size_t i = 0; i < buffer.getNumFrames(); i++) {
        leftPhase += lStep;
        rightPhase += rStep;
        tablePhase += tStep;

        // Generate synthesized tones (Sine + subtle harmonic saturation)
        float lSample = (sin(leftPhase) + 0.2f * sin(leftPhase * 2.0)) * lAmp;
        float rSample = (sin(rightPhase) + 0.2f * sin(rightPhase * 2.0)) * rAmp;
        float tSample = sin(tablePhase) * tAmp;

        // Left speaker: Left Racket + 50% Table Hit
        buffer[i * 2 + 0] = (lSample + tSample * 0.5f) * masterVol;
        // Right speaker: Right Racket + 50% Table Hit
        buffer[i * 2 + 1] = (rSample + tSample * 0.5f) * masterVol;
    }
}

void ofApp::draw() {
    ofEnableDepthTest();
    cam.begin();

    // Draw 3D Grid Floor
    ofPushStyle();
    ofSetColor(45, 50, 62, 120);
    ofDrawGrid(400, 10, false, false, true, false);
    ofPopStyle();

    // Draw Left & Right 3D Rackets
    ofPushMatrix();
    ofTranslate(-220, 0, 0);
    drawRacketModel(leftRacket.roll, leftRacket.pitch, leftRacket.yaw, ofColor(255, 80, 100), "Left Lover (L)");
    ofPopMatrix();

    ofPushMatrix();
    ofTranslate(220, 0, 0);
    drawRacketModel(rightRacket.roll, rightRacket.pitch, rightRacket.yaw, ofColor(80, 240, 210), "Right Lover (R)");
    ofPopMatrix();

    // Draw Ping Pong Table Center
    ofPushMatrix();
    ofTranslate(0, -60, 0);
    ofPushStyle();
    ofSetColor(35, 80, 170, 170);
    ofDrawBox(280, 10, 160);
    ofSetColor(240, 240, 240, 200);
    ofDrawBox(0, 10, 0, 4, 30, 160); // Net
    ofPopStyle();
    ofPopMatrix();

    cam.end();
    ofDisableDepthTest();

    // Draw 2D UI & Oscilloscopes
    drawLedStrip(360, 20, ofGetWidth() - 400, 60);
    
    int oscW = 380;
    int oscH = 90;
    int oscX = ofGetWidth() - oscW - 20;
    
    drawOscilloscope(oscX, 90, oscW, oscH, leftPiezoHistory, 0, 100, ofColor(255, 80, 100), "Left Piezo Spikes");
    drawOscilloscope(oscX, 190, oscW, oscH, rightPiezoHistory, 0, 100, ofColor(80, 240, 210), "Right Piezo Spikes");
    drawOscilloscope(oscX, 290, oscW, oscH, leftSwingHistory, 0, 60, ofColor(255, 140, 80), "Left Swing Speed");
    drawOscilloscope(oscX, 390, oscW, oscH, rightSwingHistory, 0, 60, ofColor(120, 220, 255), "Right Swing Speed");

    drawTelemetryDashboard();
    gui.draw();
}

void ofApp::drawLedStrip(int x, int y, int w, int h) {
    ofPushStyle();
    ofSetColor(24, 28, 36, 230);
    ofDrawRectangle(x, y, w, h);

    ofSetColor(200);
    if (fontSmall.isLoaded()) {
        fontSmall.drawString("Mittellinie / Center Line LED Strip (58 LEDs)", x + 18, y + 26);
    } else {
        ofDrawBitmapString("Mittellinie / Center Line LED Strip (58 LEDs)", x + 15, y + 18);
    }

    float ledSpacing = (w - 30.0f) / 58.0f;
    for (int i = 0; i < 58; i++) {
        float ledX = x + 15 + (i + 0.5f) * ledSpacing;
        float ledY = y + 44;

        auto& pixel = ledStrip[i];
        ofColor renderColor = pixel.color;
        renderColor.setBrightness((int)(pixel.brightness * 255.0f));
        if (renderColor.getBrightness() < 30) {
            renderColor = ofColor(35, 40, 50); // Off state
        }

        ofSetColor(renderColor);
        ofDrawCircle(ledX, ledY, ledSpacing * 0.4f);
    }
    ofPopStyle();
}

void ofApp::drawOscilloscope(int x, int y, int w, int h, const std::vector<float>& data, float minValue, float maxValue, const ofColor& color, const string& title) {
    ofPushStyle();
    ofSetColor(22, 26, 34, 230);
    ofDrawRectangle(x, y, w, h);

    ofSetColor(color);
    if (fontOsc.isLoaded()) {
        fontOsc.drawString(title, x + 12, y + 22);
    } else {
        ofDrawBitmapString(title, x + 10, y + 16);
    }

    // Draw signal boundary box & grid
    ofSetColor(60, 65, 75, 150);
    ofNoFill();
    ofDrawRectangle(x + 5, y + 30, w - 10, h - 36);
    ofDrawLine(x + 5, y + 30 + (h - 36) * 0.5f, x + w - 5, y + 30 + (h - 36) * 0.5f);

    // Draw signal waveform
    if (!data.empty()) {
        ofSetColor(color);
        ofSetLineWidth(1.5f);
        ofPolyline line;
        float stepX = (w - 10.0f) / (float)data.size();
        float graphY = y + 30 + (h - 36);
        float graphH = (h - 36);

        for (size_t i = 0; i < data.size(); i++) {
            float val = ofClamp(data[i], minValue, maxValue);
            float norm = (val - minValue) / (maxValue - minValue + 0.0001f);
            float ptX = x + 5 + i * stepX;
            float ptY = graphY - norm * graphH;
            line.addVertex(ptX, ptY);
        }
        line.draw();
    }
    ofPopStyle();
}

void ofApp::drawRacketModel(float roll, float pitch, float yaw, const ofColor& color, const string& label) {
    ofPushMatrix();
    ofRotateZDeg(-roll);
    ofRotateXDeg(pitch);
    ofRotateYDeg(yaw);

    ofPushStyle();
    ofSetColor(color.getBrightness() * 0.6f);
    ofDrawBox(0, -70, 0, 14, 60, 14);

    ofSetColor(color);
    ofDrawBox(0, 0, 0, 90, 100, 12);

    ofSetColor(30, 30, 35);
    ofDrawBox(0, 0, 0, 80, 90, 13);
    ofPopStyle();
    ofPopMatrix();

    ofDisableDepthTest();
    ofPushStyle();
    ofSetColor(color);
    if (fontTitle.isLoaded()) {
        fontTitle.drawString(label, -60, 110);
    } else {
        ofDrawBitmapStringHighlight(label, glm::vec3(-50, 100, 0), ofColor(15, 18, 24, 220), color);
    }
    ofPopStyle();
    ofEnableDepthTest();
}

void ofApp::drawTelemetryDashboard() {
    int x = 20;
    int y = ofGetHeight() - 185;

    ofPushStyle();
    ofSetColor(24, 28, 36, 230);
    ofDrawRectangle(x, y, ofGetWidth() - 420, 165);

    ofSetColor(255);
    string statsText = "--- REAL-TIME AUDIO-VISUAL DEBUGGER ENGINE (120 FPS Native C++) ---\n";
    statsText += "Left Racket:  PPS=" + ofToString(leftRacket.pps) + " | Piezo=" + ofToString(leftRacket.piezo) + " (Peak: " + ofToString(leftRacket.hitPeak) + ") | Swing=" + ofToString(leftRacket.swingSpeed, 1) + " | Grip=" + ofToString(leftRacket.gripPressure) + "\n";
    statsText += "Right Racket: PPS=" + ofToString(rightRacket.pps) + " | Piezo=" + ofToString(rightRacket.piezo) + " (Peak: " + ofToString(rightRacket.hitPeak) + ") | Swing=" + ofToString(rightRacket.swingSpeed, 1) + " | Grip=" + ofToString(rightRacket.gripPressure) + "\n";
    statsText += "Table Hits:   Left=" + ofToString(tableData.leftHitCount) + " | Right=" + ofToString(tableData.rightHitCount) + " | Net=" + ofToString(tableData.netHitCount) + " | Rally=" + ofToString(gameData.rallyCount);

    if (fontTitle.isLoaded() && fontStats.isLoaded()) {
        ofSetColor(255, 220, 100);
        fontTitle.drawString("--- REAL-TIME AUDIO-VISUAL DEBUGGER ENGINE (120 FPS Native C++) ---", x + 20, y + 32);
        ofSetColor(240, 245, 255);
        string lines = "Left Racket : PPS=" + ofToString(leftRacket.pps) + " | Piezo=" + ofToString(leftRacket.piezo) + " (Peak: " + ofToString(leftRacket.hitPeak) + ") | Swing=" + ofToString(leftRacket.swingSpeed, 1) + " | Grip=" + ofToString(leftRacket.gripPressure) + "\n";
        lines += "Right Racket: PPS=" + ofToString(rightRacket.pps) + " | Piezo=" + ofToString(rightRacket.piezo) + " (Peak: " + ofToString(rightRacket.hitPeak) + ") | Swing=" + ofToString(rightRacket.swingSpeed, 1) + " | Grip=" + ofToString(rightRacket.gripPressure) + "\n";
        lines += "Table Hits  : Left=" + ofToString(tableData.leftHitCount) + " | Right=" + ofToString(tableData.rightHitCount) + " | Net=" + ofToString(tableData.netHitCount) + " | Rally=" + ofToString(gameData.rallyCount);
        fontStats.drawString(lines, x + 20, y + 68);
    } else {
        ofDrawBitmapString(statsText, x + 15, y + 25);
    }
    ofPopStyle();
}

void ofApp::exit() {
    soundStream.close();
    serial.close();
}

void ofApp::keyPressed(int key) {
    if (key == 'r' || key == 'R') {
        connectSerial();
    }
}

void ofApp::windowResized(int w, int h) {}
