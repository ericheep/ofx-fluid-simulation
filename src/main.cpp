#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main() {
	ofGLWindowSettings settings;

    settings.setSize(540, 960);
    settings.setGLVersion(4,1);

    // settings.windowMode = OF_FULLSCREEN;
    auto window = ofCreateWindow(settings);
    
	ofRunApp(window, make_shared<ofApp>());
	ofRunMainLoop();
}
