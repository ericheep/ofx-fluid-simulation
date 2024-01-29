#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main() {
	ofGLWindowSettings settings;
    // settings.setSize(1360, 880);
    // settings.setSize(500, 500);
    settings.setSize(540, 960);
    settings.setGLVersion(4,1);
    // settings.setSize(1280, 1024);
	// settings.windowMode = OF_FULLSCREEN;
    auto window = ofCreateWindow(settings);
    
	ofRunApp(window, make_shared<ofApp>());
	ofRunMainLoop();
}
