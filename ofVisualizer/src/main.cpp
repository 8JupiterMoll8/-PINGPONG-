#include "ofMain.h"
#include "ofApp.h"

int main() {
    // Setup OpenGL window at 1280x800, using vertical sync
    ofSetupOpenGL(1280, 800, OF_WINDOW);
    ofRunApp(new ofApp());
}
