#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    oscReceiver.setup(RECEIVING_PORT);
    
    //systemWidth = 1920;
    //systemHeight = 1200;
    systemWidth = 1080;
    systemHeight = 1920;
    systemFbo.allocate(systemWidth, systemHeight);
    individualTextureSyphonServer.setName("fbo texture output");
    ofSetFrameRate(60);

    // gui setup
    ofxGuiSetFont("DankMono-Bold.ttf", 10);
    ofxGuiSetBorderColor(16);
    gui.setup("fluid sim");
    gui.setSize(140, 12);
    gui.setDefaultWidth(120);
    gui.setDefaultHeight(12);
    
    // general gui settings
    numberParticles.addListener(this, &ofApp::setNumberParticles);
    gui.add(numberParticles.set("number", 45000, 50, 100000));
    influenceRadius.addListener(this, &ofApp::setInfluenceRadius);
    gui.add(influenceRadius.set("influence radius", 4.0, 0.5, 25.0));
    timeScalar.addListener(this, &ofApp::setTimeScalar);
    gui.add(timeScalar.set("time scalar", 1.0, 0.5, 4.0));
    gravityMultiplier.addListener(this, &ofApp::setGravityMultiplier);
    gui.add(gravityMultiplier.set("gravity multiplier", 1.0, 0.0, 5.0));
    gui.add(gravityRotationIncrement.setup("gravity rotation", 1.0, 0.0, 5.0));
    
    // graphic gui settings
    drawMode.addListener(this, &ofApp::setDrawMode);
    gui.add(drawMode.set("draw mode", 0, 0, 5));
    velocityCurve.addListener(this, &ofApp::setVelocityCurve);
    gui.add(velocityCurve.set("velocity curve", 1.0, 0.0, 3.0));
    minVelocity.addListener(this, &ofApp::setMinVelocity);
    gui.add(minVelocity.set("min velocity", 0.0, 0.0, 100.0));
    maxVelocity.addListener(this, &ofApp::setMaxVelocity);
    gui.add(maxVelocity.set("max velocity", 50.0, 0.0, 250.0));
    minSize.addListener(this, &ofApp::setMinSize);
    gui.add(minSize.set("min size", 0.0, 0.0, 50.0));
    maxSize.addListener(this, &ofApp::setMaxSize);
    gui.add(maxSize.set("max size", 8.0, 0.0, 50.0));
    mouseRadius.addListener(this, &ofApp::setMouseRadius);
    gui.add(mouseRadius.set("mouse radius", 200, 10, 500));
    mouseForce.addListener(this, &ofApp::setMouseForce);
    gui.add(mouseForce.set("mouse force", 15.0, 1., 100.));
    lineThickness.addListener(this, &ofApp::setLineThickness);
    gui.add(lineThickness.setup("line thickness", 1.0, 0.1, 15.0));
    
    // simulation gui settings
    simulationSettings.setName("sim settings");
    targetDensity.addListener(this, &ofApp::setTargetDensity);
    simulationSettings.add(targetDensity.set("density", 1.0, 0.25, 3.0));
    pressureMultiplier.addListener(this, &ofApp::setPressureMultiplier);
    simulationSettings.add(pressureMultiplier.set("pressure", 100, 0, 1000.0));
    nearPressureMultiplier.addListener(this, &ofApp::setNearPressureMultiplier);
    simulationSettings.add(nearPressureMultiplier.set("near pressure", 100, 0.0, 1000.0));
    gui.add(simulationSettings);
    
    // boundary gui settings
    boundarySettings.setName("boundary settings");
    boundsWidth.addListener(this, &ofApp::setBoundsWidth);
    boundarySettings.add(boundsWidth.set("width", systemWidth, 50, systemWidth));
    boundsHeight.addListener(this, &ofApp::setBoundsHeight);
    boundarySettings.add(boundsHeight.set("height", systemHeight, 50, systemHeight));
    borderOffset.addListener(this, &ofApp::setBorderOffset);
    boundarySettings.add(borderOffset.set("offset", 0.0, 0.0, 50.0));
    circleBoundary.addListener(this, &ofApp::setCircleBoundary);
    boundarySettings.add(circleBoundary.set("circle boundary", true));
    gui.add(boundarySettings);
   
    // cool color gui setup
    coolColorGui.setup("cool");
    coolColorGui.setPosition(ofGetWidth() - 260, 10);
    coolColorGui.setDefaultWidth(120);
    coolColorGui.setDefaultHeight(12);
    coolColorGui.add(coolColor.setup(ofColor::lightSeaGreen, 120, 12));
    coolColor.disableHeader();
    coolColor.maximize();
    
    // hot color gui setup
    hotColorGui.setup("hot");
    hotColorGui.setPosition(ofGetWidth() - 130, 10);
    hotColorGui.setDefaultWidth(120);
    hotColorGui.setDefaultHeight(12);
    hotColorGui.add(hotColor.setup(ofColor::lightPink, 120, 12));
    hotColor.disableHeader();
    hotColor.maximize();
    
    // reset buttons
    resetRandomButton.addListener(this, &ofApp::resetRandom);
    gui.add(resetRandomButton.setup("reset random", 20, 12));
    resetGridButton.addListener(this, &ofApp::resetGrid);
    gui.add(resetGridButton.setup("reset grid", 20, 12));
    
    // close seldom used windows
    gui.minimizeAll();
    gui.maximize();
    
    // simulation settings
    fluidSystem.setWidth(systemWidth);
    fluidSystem.setHeight(systemHeight);
    fluidSystem.setBoundsSize(ofVec3f(boundsWidth, boundsHeight, 0));
    fluidSystem.setCenter(systemWidth * 0.5, systemHeight * 0.5);
    fluidSystem.setCollisionDamping(0.05);
    fluidSystem.setViscosityStrength(0.25);
    fluidSystem.setMode(0);
    fluidSystem.setNumberParticles(numberParticles);
    fluidSystem.resetRandom();
    
    // misc settings
    cam.setupPerspective();
    backgroundColor = ofColor::black;
    gravityRotation = ofVec2f(0.0, 1.0);
}

//--------------------------------------------------------------
void ofApp::update() {
    checkIncomingOsc();

    fluidSystem.setCoolColor(coolColor);
    fluidSystem.setHotColor(hotColor);
    
    widthRatio = boundsWidth / float(ofGetWidth());
    heightRatio = boundsHeight / float(ofGetHeight());
    
    gravityRotation = gravityRotation.rotate(gravityRotationIncrement);
    fluidSystem.setGravityRotation(gravityRotation);
    
    fluidSystem.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    systemFbo.begin();
    ofClear(0, 0, 0);
    
    if (fluidSystem.exportFrameActive) {
        string filename = to_string(numberParticles) + "-" + ofGetTimestampString("%F") + ".svg";
        ofBeginSaveScreenAsSVG(filename);
        
        ofSetColor(ofColor::black);
        fluidSystem.setCoolColor(ofColor::black);
        fluidSystem.setHotColor(ofColor::black);
    } else {
        ofBackground(backgroundColor);
    }
    
    fluidSystem.draw();
    systemFbo.end();
    systemFbo.draw(0, 0, ofGetWidth(), ofGetHeight());
    
    if (fluidSystem.exportFrameActive) {
        ofEndSaveScreenAsSVG();
        fluidSystem.exportFrameActive = false;
    }

    individualTextureSyphonServer.publishTexture(&systemFbo.getTexture());
    
    // guis
    gui.draw();
    coolColorGui.draw();
    hotColorGui.draw();

    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());
}

void ofApp::checkIncomingOsc() {
    while(oscReceiver.hasWaitingMessages()) {
        ofxOscMessage m;
        oscReceiver.getNextMessage(m);
        
        if (m.getAddress() == "/simulateMouse") {
            float x = ofMap(m.getArgAsFloat(0), -1.0, 1.0, 0, systemWidth);
            float y = ofMap(m.getArgAsFloat(1), -1.0, 1.0, 0, systemHeight);
            
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

// keyboard functions
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
    float scaledX = ofMap(x, 0, ofGetWidth(), 0, systemWidth);
    float scaledY = ofMap(y, 0, ofGetHeight(), 0, systemHeight);

    fluidSystem.mouseInput(scaledX, scaledY);
}

void ofApp::mousePressed(int x, int y, int button) {
    float scaledX = ofMap(x, 0, ofGetWidth(), 0, systemWidth);
    float scaledY = ofMap(y, 0, ofGetHeight(), 0, systemHeight);
    
    fluidSystem.mouseInput(scaledX, scaledY, button, true);
}

void ofApp::mouseReleased(int x, int y, int button) {
    float scaledX = ofMap(x, 0, ofGetWidth(), 0, systemWidth);
    float scaledY = ofMap(y, 0, ofGetHeight(), 0, systemHeight);
    
    fluidSystem.mouseInput(scaledX, scaledY, button, false);
}

void ofApp::windowResized(int w, int h) {
    boundsWidth = widthRatio * w;
    boundsHeight = heightRatio * h;
}

void ofApp::setDrawMode(int & drawMode) {
    // 0 = circles
    // 1 = rectangles
    // 2 = vectors
    // 3 = lines
    // 4 = points
    
    fluidSystem.setMode(drawMode);
}

void ofApp::setNumberParticles(int & numberParticles) {
    fluidSystem.setNumberParticles(numberParticles);
    fluidSystem.setVelocityCurve(velocityCurve);
    fluidSystem.setMinSize(minSize);
    fluidSystem.setMaxSize(maxSize);
    fluidSystem.setMinVelocity(minVelocity);
    fluidSystem.setMaxVelocity(maxVelocity);
}

void ofApp::setTimeScalar(float & timeScalar) {
    fluidSystem.setDeltaTime(1.0 / 60.0 / timeScalar);
}

void ofApp::setInfluenceRadius(float & influenceRadius) {
    fluidSystem.setRadius(influenceRadius);
}

void ofApp::setGravityMultiplier(float & gravityMultiplier) {
    fluidSystem.setGravityMultiplier(gravityMultiplier);
}

void ofApp::setTargetDensity(float & targetDensity) {
    fluidSystem.setTargetDensity(targetDensity);
}

void ofApp::setPressureMultiplier(float & pressureMultiplier) {
    fluidSystem.setPressureMultiplier(pressureMultiplier);
}

void ofApp::setNearPressureMultiplier(float & nearPressureMultiplier) {
    fluidSystem.setNearPressureMultiplier(nearPressureMultiplier);
}

void ofApp::setBoundsWidth(int & boundsWidth) {
    fluidSystem.setBoundsSize(ofVec3f(boundsWidth - borderOffset, boundsHeight - borderOffset, 0));}

void ofApp::setBoundsHeight(int & boundsHeight) {
    fluidSystem.setBoundsSize(ofVec3f(boundsWidth - borderOffset, boundsHeight - borderOffset, 0));}

void ofApp::setBorderOffset(int & borderOffset) {
    fluidSystem.setBoundsSize(ofVec3f(boundsWidth - borderOffset, boundsHeight - borderOffset, 0));
}

void ofApp::setVelocityCurve(float & velocityCurve) {
    fluidSystem.setVelocityCurve(velocityCurve);
}

void ofApp::setMinVelocity(float & minVelocity) {
    fluidSystem.setMinVelocity(minVelocity);
}

void ofApp::setMaxVelocity(float & maxVelocity) {
    fluidSystem.setMaxVelocity(maxVelocity);
}

void ofApp::setMinSize(float & minSize) {
    fluidSystem.setMinSize(minSize);
}

void ofApp::setMaxSize(float & maxSize) {
    fluidSystem.setMaxSize(maxSize);
}

void ofApp::setMouseRadius(float & mouseRadius) {
    fluidSystem.setMouseRadius(mouseRadius);
}

void ofApp::setMouseForce(float & mouseForce) {
    fluidSystem.setMouseForce(mouseForce);
}

void ofApp::setCircleBoundary(bool & circleBoundary) {
    fluidSystem.setCircleBoundary(circleBoundary);
}

void ofApp::setLineThickness(float & lineThickness) {
    fluidSystem.setLineThickness(lineThickness);
}

void ofApp::resetRandom() {
    fluidSystem.resetRandom();
}

void ofApp::resetGrid() {
    fluidSystem.resetGrid(1.0);
}

void ofApp::exit(){
    // idk something
}
