#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"

#include "FluidSystem2D.hpp"
#include "FluidSystem3D.hpp"

#define RECEIVING_PORT 5432

class ofApp : public ofBaseApp{
public:
    void setup() override;
    void update() override;
    void checkIncomingOsc();
    void draw() override;
    void exit() override;
    
    void keyPressed(int key) override;
    void mouseDragged(int x, int y, int button) override;
    void mousePressed(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void windowResized(int w, int h) override;
private:
    FluidSystem2D fluidSystem;
    ofEasyCam cam;
    
    ofxOscReceiver oscReceiver;
    
    ofxPanel gui;
    ofxIntSlider numberParticles;
    ofxFloatSlider particleSize, collisionDamping, resetScale;
    ofxFloatSlider targetDensity, nearPressureMultiplier, pressureMultiplier, gravityMultiplier, timeScalar, viscosityStrength, lineWidthScalar, velocityHue, lineWidthMinimum;
    ofxIntSlider mouseRadius;
    ofxIntSlider boundsWidth, boundsHeight, boundsDepth;
    ofxButton resetRandomButton, resetGridButton, resetCircleButton;
    ofColor backgroundColor, hotColor, coolColor;
    
    Boolean simulateActive;
    
    enum MODES { MODE_SVG, MODE_2D, MODE_3D } mode;
    float width, height, depth;
    
    Boolean pauseActive;
    Boolean SVG_MODE;
    
    float widthRatio, heightRatio;
    
    void pause();
    void nextFrame();
    void saveSvg();
    
    void resetRandom();
    void resetGrid();
    void resetCircle();
};
