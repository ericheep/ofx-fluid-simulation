#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main() {
	ofGLWindowSettings settings;
	settings.setSize(1080, 1350);
    // settings.setSize(720, 950);
	settings.windowMode = OF_WINDOW;
    auto window = ofCreateWindow(settings);
    
	ofRunApp(window, make_shared<ofApp>());
	ofRunMainLoop();
}
