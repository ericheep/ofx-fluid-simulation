#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    mode = MODE_2D;
    
    oscReceiver.setup(RECEIVING_PORT);
    
    ofxGuiSetFont("DankMono-Bold.ttf", 10);
    ofxGuiSetBorderColor(16);
    gui.setup("fluid simulation");
    gui.setSize(170, 150);
    gui.setDefaultHeight(12);
    
    gui.add(boundsWidth.setup("width", ofGetWidth(), 50, ofGetWidth()));
    gui.add(boundsHeight.setup("height", ofGetHeight(), 50, ofGetHeight()));
    gui.add(boundsDepth.setup("depth", ofGetWidth(), 50, ofGetWidth()));
    
    gui.add(numberParticles.setup("number", 20000, 50, 50000));
    gui.add(gravityMultiplier.setup("gravity multiplier", 0.0, 0.0, 15.0));
    gui.add(particleSize.setup("particle size", 30.0, 0.5, 100.0));
    gui.add(targetDensity.setup("target density", 1.0, 0.1, 2.0));
    gui.add(pressureMultiplier.setup("pressure multiplier", 50, 0, 1000.0));
    gui.add(nearPressureMultiplier.setup("near pressure multiplier", 25, 0.0, 1000.0));
    gui.add(viscosityStrength.setup("viscosity strength", 0.25, 0.0, 1.0));
    gui.add(collisionDamping.setup("collision damping", 0.05, 0.1, 1.5));

    resetRandomButton.addListener(this, &ofApp::resetRandom);
    gui.add(resetRandomButton.setup("reset random", 20, 12));
    
    gui.add(resetScale.setup("reset scale", 1.0, 0.0, 1.0));
    resetGridButton.addListener(this, &ofApp::resetGrid);
    gui.add(resetGridButton.setup("reset grid", 20, 12));
    resetCircleButton.addListener(this, &ofApp::resetCircle);
    gui.add(resetCircleButton.setup("reset circle", 20, 12));
    gui.add(mouseRadius.setup("mouse radius", 150, 10, 500));
    gui.add(velocityHue.setup("velocity hue", 500, 10, 1000));
    gui.add(lineWidthScalar.setup("line width scalar", 0.05, 0.0, 3.0));
    gui.add(lineWidthMinimum.setup("line width min", 2.0, 0.0, 5.0));

    cam.setupPerspective();
    
    switch (mode) {
        case MODE_SVG:
            numberParticles = 20000;
            pressureMultiplier = 50;
            nearPressureMultiplier = 25;
            break;
        case MODE_2D:
            numberParticles = 20000;
            pressureMultiplier = 50;
            nearPressureMultiplier = 25;
            break;
        case MODE_3D:
            ofVec3f target = ofVec3f(ofGetWidth() / 2.0, ofGetHeight() / 2.0, ofGetWidth() / 2.0);
            cam.setTarget(target);
            cam.setDistance(2100);
            break;
    }
    
    ofSetFrameRate(60);
    fluidSystem.setDeltaTime(1.0 / 60.0);
    fluidSystem.setBoundsSize(ofVec3f(boundsWidth, boundsHeight, boundsDepth));
    fluidSystem.setNumberParticles(numberParticles);
    fluidSystem.resetRandom();
    
    simulateActive = false;
    timeScalar = 1.0;
}

//--------------------------------------------------------------
void ofApp::update() {
    checkIncomingOsc();
    
    switch (mode) {
        case MODE_SVG:
            backgroundColor = ofColor::white;
            fluidSystem.setCoolColor(ofColor::black);
            fluidSystem.setHotColor(ofColor::black);
            fluidSystem.setLineThickness(1);
            break;
        case MODE_2D:
            backgroundColor = ofColor::black;
            fluidSystem.setCoolColor(ofColor::blue);
            fluidSystem.setHotColor(ofColor::orangeRed);
            fluidSystem.setLineThickness(3);
            break;
        case MODE_3D:
            backgroundColor = ofColor::black;
            fluidSystem.setCoolColor(ofColor::blue);
            fluidSystem.setHotColor(ofColor::orangeRed);
            fluidSystem.setLineThickness(3);
            break;
    }
    
    widthRatio = boundsWidth / float(ofGetWidth());
    heightRatio = boundsHeight / float(ofGetHeight());
    
    fluidSystem.setNumberParticles(numberParticles);
    fluidSystem.setGravityMultiplier(gravityMultiplier);
    fluidSystem.setTargetDensity(targetDensity);
    fluidSystem.setPressureMultiplier(pressureMultiplier);
    fluidSystem.setNearPressureMultiplier(nearPressureMultiplier);
    fluidSystem.setViscosityStrength(viscosityStrength);
    fluidSystem.setCollisionDamping(collisionDamping);
    fluidSystem.setRadius(particleSize / 2.0);
    fluidSystem.setBoundsSize(ofVec3f(boundsWidth, boundsHeight, boundsDepth));
    fluidSystem.setMouseRadius(mouseRadius);
    fluidSystem.setVelocityHue(velocityHue);
    fluidSystem.setLineWidthScalar(lineWidthScalar);
    fluidSystem.setLineWidthMinimum(lineWidthMinimum);
    
    fluidSystem.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    switch (mode) {
        case MODE_SVG:
            ofBackground(backgroundColor);
            fluidSystem.draw();
            break;
        case MODE_2D:
            ofBackground(backgroundColor);
            fluidSystem.draw();
            break;
        case MODE_3D:
            cam.begin();
            ofEnableDepthTest();
            ofBackground(backgroundColor);
            fluidSystem.draw();
            ofDisableDepthTest();
            cam.end();
            break;
    }
    
    gui.draw();
    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());
}

void ofApp::resetRandom() {
    fluidSystem.resetRandom();
}

void ofApp::resetGrid() {
    fluidSystem.resetGrid(resetScale);
}

void ofApp::resetCircle() {
    fluidSystem.resetCircle(resetScale);
}

void ofApp::checkIncomingOsc() {
    while(oscReceiver.hasWaitingMessages()) {
        ofxOscMessage m;
        oscReceiver.getNextMessage(m);
        
        if (m.getAddress() == "/simulateMouse") {
            float x = ofMap(m.getArgAsFloat(0), -1.0, 1.0, 0, ofGetWidth());
            float y = ofMap(m.getArgAsFloat(1), -1.0, 1.0, 0, ofGetHeight());
            
            fluidSystem.mouseInput(x, y, 0, simulateActive);
        }
        
        if (m.getAddress() == "/simulateActive") {
            if (m.getArgAsInt(0) == 1) {
                simulateActive = true;
            } else {
                simulateActive = false;
            }
        }
    }
}

// overrides

void ofApp::keyPressed(int key) {
    if(key == 's' || key == 'S') {
        fluidSystem.saveSvg();
    }
    
    if(key == '[') {
        fluidSystem.nextFrame();
    }
    
    if(key == ']') {
        fluidSystem.nextFrame();
    }
    
    if(key == 'p') {
        pauseActive = !pauseActive;
        fluidSystem.pause(pauseActive);
    }
}

void ofApp::mouseDragged(int x, int y, int button) {
    fluidSystem.mouseInput(x, y);
}

void ofApp::mousePressed(int x, int y, int button) {
    fluidSystem.mouseInput(x, y, button, true);
}

void ofApp::mouseReleased(int x, int y, int button) {
    fluidSystem.mouseInput(x, y, button, false);
}

void ofApp::windowResized(int w, int h) {
    boundsWidth = widthRatio * w;
    boundsHeight = heightRatio * h;
}

void ofApp::exit(){
    // idk something
}
