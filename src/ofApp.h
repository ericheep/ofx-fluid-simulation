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
    
    ofShader blur;
    ofShader bloom;
    ofShader contrast;
    
    ofFbo blurFbo;
    ofFbo bloomFbo;
    ofFbo contrastFbo;

    ofxSyphonServer individualTextureSyphonServer;
    ofxOscReceiver oscReceiver;
    ofFbo systemFbo;
    
    ofxPanel gui, coolColorGui, hotColorGui, shaderGui;
    ofParameterGroup simulationSettings;
    ofParameterGroup boundarySettings;
    
    ofParameter<int> drawMode;
    ofParameter<int> numberParticles;
    ofParameter<float> timeScalar;
    ofParameter<float> influenceRadius;
    ofParameter<float> gravityMultiplier;
    ofParameter<float> targetDensity;
    ofParameter<float> pressureMultiplier;
    ofParameter<float> nearPressureMultiplier;
    
    ofParameter<int> boundsWidth, boundsHeight;
    ofParameter<int> borderOffset;
    
    ofParameter<float> velocityCurve;
    ofParameter<float> minVelocity, maxVelocity;
    ofParameter<float> minSize, maxSize;
    
    ofParameter<float> mouseForce, mouseRadius;
    ofParameter<bool> circleBoundary;

    ofxFloatSlider lineThickness;
    ofxFloatSlider centerX, centerY, gravityRotationIncrement;
    ofxFloatSlider blurQuality, blurAngles, blurRadius, blurMix;
    ofxFloatSlider contrastAmount;

    ofxButton resetRandomButton, resetGridButton;
    ofxColorSlider coolColor, hotColor, backgroundColor;
            
    // syphon output settings
    int systemWidth, systemHeight;
    float widthRatio, heightRatio;
    ofVec2f gravityRotation;
    Boolean pauseActive;
    Boolean simulateActive;
    
    // keyboard functions
    void pause();
    void nextFrame();
    void saveSvg();
    
    // gui listener functions
    void setDrawMode(int & drawMode);
    void setNumberParticles(int & numberParticles);
    void setTimeScalar(float & timeScalar);
    void setInfluenceRadius(float & influenceRadius);
    void setGravityMultiplier(float & gravityMultiplier);
    void setTargetDensity(float & targetDensity);
    void setPressureMultiplier(float & pressureMultiplier);
    void setNearPressureMultiplier(float & nearPressureMultiplier);
    void setCoolColor(ofColor & coolColor);
    void setHotColor(ofColor & hotColor);
    
    // gui boundary listener functions
    void setBoundsWidth(int & boundsWidth);
    void setBoundsHeight(int & boundsHeight);
    void setBorderOffset(int & borderOffset);
    void setCircleBoundary(bool & circleBoundary);
    
    // gui graphic listener functions
    void setVelocityCurve(float & velocityCurve);
    void setMinVelocity(float & minVelocity);
    void setMaxVelocity(float & maxVelocity);
    void setMinSize(float & minSize);
    void setMaxSize(float & maxSize);
    void setLineThickness(float & lineThickness);
    
    // gui mouse listener functions
    void setMouseRadius(float & mouseRadius);
    void setMouseForce(float & mouseForce);
    
    void resetRandom();
    void resetGrid();
    void resetCircle();
};
