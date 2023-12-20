#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    SVG_MODE = true;
    
    ofxGuiSetFont("DankMono-Bold.ttf", 10);
    ofxGuiSetBorderColor(16);
    gui.setup("fluid simulation");
    gui.setSize(170, 150);
    gui.setDefaultHeight(12);
    
    gui.add(numberParticles.setup("number", 25000, 50, 50000));
    gui.add(gravityMultiplier.setup("gravity multiplier", 0.0, 0.0, 15.0));
    gui.add(particleSize.setup("particle size", 17.0, 0.5, 100.0));
    gui.add(targetDensity.setup("target density", 1.0, 0.1, 2.0));
    gui.add(pressureMultiplier.setup("pressure multiplier", 400, 0, 1000.0));
    gui.add(nearPressureMultiplier.setup("near pressure multiplier", 300, 0.0, 1000.0));
    gui.add(viscosityStrength.setup("viscosity strength", 0.25, 0.0, 1.0));
    gui.add(collisionDamping.setup("collision damping", 0.05, 0.1, 1.5));

    // settings for pen plotting
    if (SVG_MODE) {
        gui.add(boundingBoxWidth.setup("width", 850, 50, ofGetWidth()));
        gui.add(boundingBoxHeight.setup("height", 1100, 50, ofGetHeight()));
    } else {
        gui.add(boundingBoxWidth.setup("width", ofGetWidth(), 50, ofGetWidth()));
        gui.add(boundingBoxHeight.setup("height", ofGetHeight(), 50, ofGetHeight()));
    }

    resetRandomButton.addListener(this, &ofApp::resetRandom);
    gui.add(resetRandomButton.setup("reset random", 20, 12));
    
    gui.add(resetScale.setup("reset scale", 1.0, 0.0, 1.0));
    resetGridButton.addListener(this, &ofApp::resetGrid);
    gui.add(resetGridButton.setup("reset grid", 20, 12));
    resetCircleButton.addListener(this, &ofApp::resetCircle);
    gui.add(resetCircleButton.setup("reset circle", 20, 12));
    gui.add(mouseRadius.setup("mouse radius", 150, 10, 500));
    gui.add(velocityHue.setup("velocity hue", 500, 10, 1000));
    gui.add(lineWidthScalar.setup("line width scalar", 0.1, 0.0, 2.5));
    gui.add(lineWidthMinimum.setup("line width min", 0.0, 0.0, 5.0));

    ofSetFrameRate(60);
    fluidSystem.setDeltaTime(1.0 / 30.0);
    fluidSystem.setBoundingBox(ofVec2f(boundingBoxWidth, boundingBoxHeight));
    fluidSystem.setNumberParticles(numberParticles);
    fluidSystem.resetRandom();
    
    timeScalar = 1.0;
}

//--------------------------------------------------------------
void ofApp::update() {
    if (SVG_MODE) {
        backgroundColor = ofColor::white;
        fluidSystem.setCoolColor(ofColor::black);
        fluidSystem.setHotColor(ofColor::black);
        fluidSystem.setLineThickness(1);
    } else {
        backgroundColor = ofColor::black;
        fluidSystem.setCoolColor(ofColor::blue);
        fluidSystem.setHotColor(ofColor::orangeRed);
        fluidSystem.setLineThickness(3);
    }
    
    widthRatio = boundingBoxWidth / float(ofGetWidth());
    heightRatio = boundingBoxHeight / float(ofGetHeight());
    
    fluidSystem.setNumberParticles(numberParticles);
    fluidSystem.setGravityMultiplier(gravityMultiplier);
    fluidSystem.setTargetDensity(targetDensity);
    fluidSystem.setPressureMultiplier(pressureMultiplier);
    fluidSystem.setNearPressureMultiplier(nearPressureMultiplier);
    fluidSystem.setViscosityStrength(viscosityStrength);
    fluidSystem.setCollisionDamping(collisionDamping);
    fluidSystem.setRadius(particleSize / 2.0);
    fluidSystem.setBoundingBox(ofVec2f(boundingBoxWidth, boundingBoxHeight));
    fluidSystem.setMouseRadius(mouseRadius);
    fluidSystem.setVelocityHue(velocityHue);
    fluidSystem.setLineWidthScalar(lineWidthScalar);
    fluidSystem.setLineWidthMinimum(lineWidthMinimum);
    
    fluidSystem.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(backgroundColor);

    fluidSystem.draw();
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
    boundingBoxWidth = widthRatio * w;
    boundingBoxHeight = heightRatio * h;
}

void ofApp::exit(){
    // idk something
}
