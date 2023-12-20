#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "FluidSystem2D.hpp"

class ofApp : public ofBaseApp{
public:
    void setup() override;
    void update() override;
    void draw() override;
    void exit() override;
    
    void keyPressed(int key) override;
    void mouseDragged(int x, int y, int button) override;
    void mousePressed(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void windowResized(int w, int h) override;
private:
    FluidSystem2D fluidSystem;

    ofxPanel gui;
    ofxIntSlider numberParticles;
    ofxFloatSlider particleSize, collisionDamping, resetScale;
    ofxFloatSlider targetDensity, nearPressureMultiplier, pressureMultiplier, gravityMultiplier, timeScalar, viscosityStrength, lineWidthScalar, velocityHue, lineWidthMinimum;
    ofxIntSlider boundingBoxWidth, boundingBoxHeight, mouseRadius;
    ofxButton resetRandomButton, resetGridButton, resetCircleButton;
    ofColor backgroundColor, hotColor, coolColor;
    
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
