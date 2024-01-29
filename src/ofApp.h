#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxSyphon.h"

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

    ofxSyphonServer individualTextureSyphonServer;
    ofxOscReceiver oscReceiver;
    ofShader bloom;
    ofFbo systemFbo;
    
    ofxPanel gui;
    ofxIntSlider numberParticles, drawMode;
    ofxFloatSlider particleSize, collisionDamping, resetScale;
    ofxFloatSlider targetDensity, nearPressureMultiplier, pressureMultiplier, gravityMultiplier, timeScalar, viscosityStrength;
    ofxFloatSlider velocityHue, bloomSpread, bloomIntensity, lineThickness, coolHue, hotHue, saturation, minVelocity, maxVelocity, velocityCurve, minSize, maxSize;
    ofxFloatSlider centerX, centerY, gravityRotationIncrement, borderOffset, mouseForce, mouseRadius;
    ofxIntSlider boundsWidth, boundsHeight, boundsDepth;
    ofxButton resetRandomButton, resetGridButton, resetCircleButton;
    ofxToggle circleBoundary;
    
    ofColor backgroundColor, hotColor, coolColor;
    ofVec2f gravityRotation;
    
    Boolean simulateActive;
        
    float width, height, depth;
    int systemWidth, systemHeight, previousDrawMode;
    
    Boolean pauseActive;
    
    float widthRatio, heightRatio;
    
    void pause();
    void nextFrame();
    void saveSvg();
    
    void resetRandom();
    void resetGrid();
    void resetCircle();
    
    void updateMode();
    
    void updateGuiParameters();
};
