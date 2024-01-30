#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    // mode = MODE_OTHER;

    // systemWidth = 1920;
    // systemHeight = 1200;
    systemWidth = 1080;
    systemHeight = 1920;
    // systemWidth = ofGetWidth();
    // systemHeight = ofGetHeight();

    systemFbo.allocate(systemWidth, systemHeight);
    individualTextureSyphonServer.setName("fbo texture output");
    
    bloom.load("shaders/bloom");
    oscReceiver.setup(RECEIVING_PORT);
    
    ofxGuiSetFont("DankMono-Bold.ttf", 10);
    ofxGuiSetBorderColor(16);
    gui.setup("fluid simulation");
    gui.setSize(170, 170);
    gui.setDefaultHeight(12);
    gui.add(numberParticles.setup("number", 10000, 50, 50000));
    
    gui.add(drawMode.setup("draw mode", 0, 0, 4));
    gui.add(particleSize.setup("particle size", 15.0, 0.5, 50.0));
    gui.add(timeScalar.setup("time scalar", 1.0, 0.5, 4.0));

    gui.add(circleBoundary.setup("circle boundary", true));
    gui.add(boundsWidth.setup("width", systemWidth, 50, systemWidth));
    gui.add(boundsHeight.setup("height", systemHeight, 50, systemHeight));
    gui.add(borderOffset.setup("offset", 0.0, 0.0, 50.0));

    // gui.add(boundsDepth.setup("depth", ofGetWidth(), 50, ofGetWidth()));

    gui.add(gravityMultiplier.setup("gravity multiplier", 1.0, 0.0, 5.0));
    gui.add(gravityRotationIncrement.setup("gravity rotation inc", 1.0, 0.0, 5.0));
    gui.add(targetDensity.setup("target density", 2.0, 0.25, 3.0));
    gui.add(pressureMultiplier.setup("pressure multiplier", 500, 0, 1000.0));
    gui.add(nearPressureMultiplier.setup("near pressure multiplier", 100, 0.0, 1000.0));
    gui.add(viscosityStrength.setup("viscosity strength", 0.25, 0.0, 1.0));
    gui.add(collisionDamping.setup("collision damping", 0.05, 0.1, 1.5));

    resetRandomButton.addListener(this, &ofApp::resetRandom);
    gui.add(resetRandomButton.setup("reset random", 20, 12));
    
    gui.add(resetScale.setup("reset scale", 1.0, 0.0, 1.0));
    
    resetGridButton.addListener(this, &ofApp::resetGrid);
    gui.add(resetGridButton.setup("reset grid", 20, 12));
        
    resetCircleButton.addListener(this, &ofApp::resetCircle);
    gui.add(resetCircleButton.setup("reset circle", 20, 12));
    gui.add(mouseRadius.setup("mouse radius", 250, 10, 500));
    gui.add(mouseForce.setup("mouse force", 50.0, 1., 100.));
    gui.add(velocityCurve.setup("velocity curve", 1.0, 0.0, 3.0));
    gui.add(minVelocity.setup("min velocity", 0.0, 0.0, 100.0));
    gui.add(maxVelocity.setup("max velocity", 50.0, 0.0, 250.0));
    gui.add(minSize.setup("min size", 0.0, 0.0, 50.0));
    gui.add(maxSize.setup("max size", 8.0, 0.0, 50.0));
    gui.add(lineThickness.setup("line thickness", 1.0, 0.1, 15.0));
    gui.add(coolHue.setup("cool hue", 120.0, 0.0, 255.0));
    gui.add(hotHue.setup("hot hue", 255.0, 0.0, 255.0));
    gui.add(saturation.setup("saturation", 60.0, 0.0, 255.0));
    gui.add(centerX.setup("center X", systemWidth / 2.0, 0.0, systemWidth));
    gui.add(centerY.setup("center Y", systemHeight / 2.0, 0.0, systemHeight));

    cam.setupPerspective();
    
    ofSetFrameRate(60);
    fluidSystem.setWidth(systemWidth);
    fluidSystem.setHeight(systemHeight);
    fluidSystem.setDeltaTime(1.0 / 60.0);
    fluidSystem.setBoundsSize(ofVec3f(boundsWidth, boundsHeight, boundsDepth));
    fluidSystem.setNumberParticles(numberParticles);
    fluidSystem.setMode(0);
    fluidSystem.resetCircle(0.8);
    
    simulateActive = true;
    gravityRotation = ofVec2f(1.0, 0.0);

    backgroundColor = ofColor::black;

    updateGuiParameters();
    
    /*switch (mode) {
        case MODE_SVG:
            backgroundColor = ofColor::white;
            break;
        case MODE_OTHER:
            backgroundColor = ofColor::black;
            break;
    }*/
}

//--------------------------------------------------------------
void ofApp::update() {
    checkIncomingOsc();
    updateMode();
    
    coolColor.setHsb(coolHue, saturation, 255.0);
    hotColor.setHsb(hotHue, saturation, 255.0);

    /* switch (mode) {
        case MODE_SVG:
            fluidSystem.setCoolColor(ofColor::black);
            fluidSystem.setHotColor(ofColor::black);
            break;
        case MODE_OTHER:
            fluidSystem.setCoolColor(coolColor);
            fluidSystem.setHotColor(hotColor);
            break;
    }*/
    fluidSystem.setCoolColor(coolColor);
    fluidSystem.setHotColor(hotColor);
    widthRatio = boundsWidth / float(ofGetWidth());
    heightRatio = boundsHeight / float(ofGetHeight());
    
    updateGuiParameters();
    
    gravityRotation = gravityRotation.rotate(gravityRotationIncrement);
    
    fluidSystem.setGravityRotation(gravityRotation);
    fluidSystem.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    systemFbo.begin();
    // ofClear(0.0f, 0.0f, 0.0f, 0.0f);
    
    if (fluidSystem.exportFrameActive) {
        string filename = to_string(numberParticles) + "-" + ofGetTimestampString("%F") + ".svg";
        ofBeginSaveScreenAsSVG(filename);
    } else {
        ofBackground(backgroundColor);
    }
    
    // draw mesh
    fluidSystem.draw();
    
    if (fluidSystem.exportFrameActive) {
        ofEndSaveScreenAsSVG();
        fluidSystem.exportFrameActive = false;
    }
    
    systemFbo.end();
    systemFbo.draw(0, 0, ofGetWidth(), ofGetHeight());

    individualTextureSyphonServer.publishTexture(&systemFbo.getTexture());
    
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
        
        if (m.getAddress() == "/simulateResetCircle") {
            resetCircle();
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

void ofApp::updateGuiParameters() {
    fluidSystem.setDeltaTime(1.0 / 60.0 / timeScalar);
    fluidSystem.setNumberParticles(numberParticles);
    fluidSystem.setGravityMultiplier(gravityMultiplier);
    fluidSystem.setTargetDensity(targetDensity);
    fluidSystem.setPressureMultiplier(pressureMultiplier);
    fluidSystem.setNearPressureMultiplier(nearPressureMultiplier);
    fluidSystem.setViscosityStrength(viscosityStrength);
    fluidSystem.setCollisionDamping(collisionDamping);
    fluidSystem.setRadius(particleSize / 2.0);
    fluidSystem.setBoundsSize(ofVec3f(boundsWidth - borderOffset, boundsHeight - borderOffset, boundsDepth));
    fluidSystem.setMouseRadius(mouseRadius);
    fluidSystem.setMouseForce(mouseForce);
    fluidSystem.setMinVelocity(minVelocity);
    fluidSystem.setMaxVelocity(maxVelocity);
    fluidSystem.setMinSize(minSize);
    fluidSystem.setMaxSize(maxSize);
    fluidSystem.setVelocityCurve(velocityCurve);
    fluidSystem.setLineThickness(lineThickness);
    fluidSystem.setCenter(centerX, centerY);
    fluidSystem.setCircleBoundary(circleBoundary);
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

void ofApp::updateMode() {
    // 0 = circles
    // 1 = rectangles
    // 2 = vectors
    // 3 = lines
    // 4 = points
    
    if (previousDrawMode != drawMode) {
        fluidSystem.setMode(drawMode);
        previousDrawMode = drawMode;
    }
}

void ofApp::exit(){
    // idk something
}
