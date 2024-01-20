//
//  ParticleSystem.cpp
//  fluidSimulation
//

#include "ParticleSystem.hpp"

ParticleSystem::ParticleSystem() {
    radius = 10;
    predictionFactor = 1.0f / 120.0f;
    pressureMultiplier = 1.0;
    nearPressureMultiplier = 1.0;
    targetDensity = 1.0;
    viscosityStrength = 0.5;
    gravityConstant = 9.8; //meters per second
    gravityMultiplier = 1.0;
    collisionDamping = 0.26;
    pauseActive = false;
    gravityForce = ofVec2f(1.0, 0.0);
    
    mesh.setMode(OF_PRIMITIVE_POINTS);
    mesh.enableIndices();

}

void ParticleSystem::draw() {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].draw();
    }
}

// create particles

void ParticleSystem::addParticle() {
    float x = ofRandom(bounds.x, bounds.x + boundsSize.x);
    float y = ofRandom(bounds.y, bounds.y + boundsSize.y);
    float z = ofRandom(bounds.z, bounds.z + boundsSize.z);
    addParticle(ofVec3f(x, y, z));
}

void ParticleSystem::addParticle(ofVec3f position) {
    particles.push_back(Particle(position, radius));
    spatialLookup.resize(particles.size());
    startIndices.resize(particles.size());
}

ofVec2f ParticleSystem::getRandom2DDirection() {
    ofVec2f randomDirection = ofVec2f(1.0, 0.0).rotateRad(ofRandom(0, TWO_PI));
    return randomDirection;
}

ofVec3f ParticleSystem::getRandom3DDirection() {
    ofVec3f unitVector = ofVec3f(1.0, 0.0, 0.0);
    
    float rX = ofRandom(0, TWO_PI);
    float rY = ofRandom(0, TWO_PI);
    float rZ = ofRandom(0, TWO_PI);

    ofVec3f randomDirection = unitVector.rotateRad(rX, rY, rZ);
    
    return randomDirection;
}

void ParticleSystem::pause(Boolean _pauseActive) {
    pauseActive = _pauseActive;
}

void ParticleSystem::nextFrame() {
    nextFrameActive = true;
}

void ParticleSystem::saveSvg() {
    exportFrameActive = true;
}

// mouse input

void ParticleSystem::mouseInput(int x, int y, int button, Boolean active) {
    mouseInputActive = active;
    mouseButton = button;
    mouseInput(x, y);
}

void ParticleSystem::mouseInput(int x, int y) {
    mousePosition = ofVec2f(x, y);
}

// setters

void ParticleSystem::setNumberParticles(int number) {
    if (number > particles.size()) {
        while (particles.size() < number) {
            addParticle();
        }
        spatialLookup.resize(particles.size());
        startIndices.resize(particles.size());
    }
    if (number < particles.size()) {
        while (particles.size() > number) {
            particles.pop_back();
        }
        spatialLookup.resize(particles.size());
        startIndices.resize(particles.size());
    }
}

void ParticleSystem::setBoundsSize(ofVec3f _boundsSize) {
    center.x = ofGetWidth() / 2.0;
    center.y = ofGetHeight() / 2.0;
    boundsSize = _boundsSize;
    bounds.x = ofGetWidth() / 2.0 - boundsSize.x / 2.0;
    bounds.y = ofGetHeight() / 2.0  - boundsSize.y / 2.0;
    bounds.z = ofGetWidth() / 2.0  - boundsSize.z / 2.0;
    
    xBounds = ofVec2f(bounds.x, bounds.x + boundsSize.x);
    yBounds = ofVec2f(bounds.y, bounds.y + boundsSize.y);
    zBounds = ofVec2f(bounds.z, bounds.z + boundsSize.z);
}

void ParticleSystem::setRadius(float _radius) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].setRadius(_radius);
    }
    
    if (radius != _radius) {
        kernels.calculate3DVolumesFromRadius(_radius);
        radius = _radius;
    }
}

void ParticleSystem::setGravityRotation(ofVec2f _gravityRotation) {
    gravityForce = _gravityRotation * gravityMultiplier;
}

void ParticleSystem::setGravityMultiplier(float _gravityMultiplier) {
    gravityMultiplier = _gravityMultiplier;
}

void ParticleSystem::setDeltaTime(float _deltaTime) {
    deltaTime = _deltaTime;
}

void ParticleSystem::setCollisionDamping(float _collisionDamping) {
    collisionDamping = _collisionDamping;
}

void ParticleSystem::setTargetDensity(float _targetDensity) {
    targetDensity = _targetDensity;
}

void ParticleSystem::setPressureMultiplier(float _pressureMultiplier) {
    pressureMultiplier = _pressureMultiplier;
}

void ParticleSystem::setViscosityStrength(float _viscosityStrength) {
    viscosityStrength = _viscosityStrength;
}

void ParticleSystem::setNearPressureMultiplier(float _nearPressureMultiplier) {
    nearPressureMultiplier = _nearPressureMultiplier;
}

void ParticleSystem::setMouseRadius(int _mouseRadius) {
    mouseRadius = _mouseRadius;
}

void ParticleSystem::setCoolColor(ofColor coolColor) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].coolColor = coolColor;
    }
}

void ParticleSystem::setHotColor(ofColor hotColor) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].hotColor = hotColor;
    }
}

void ParticleSystem::setMinVelocity(float minVelocity) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].minVelocity = minVelocity;
    }
}

void ParticleSystem::setMaxVelocity(float maxVelocity) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].maxVelocity = maxVelocity;
    }
}

void ParticleSystem::setLineThickness(float lineThickness) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].lineThickness = lineThickness;
    }
}

void ParticleSystem::setVelocityCurve(float velocityCurve) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].velocityCurve = velocityCurve;
    }
}

void ParticleSystem::setMinSize(float minSize) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].minSize = minSize;
    }
}

void ParticleSystem::setMaxSize(float maxSize) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].maxSize = maxSize;
    }
}

void ParticleSystem::setMode(int mode) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].setMode(mode);
    }
}

void ParticleSystem::setCenter(float _centerX, float _centerY) {
    centerX = _centerX;
    centerY = _centerY;
}

void ParticleSystem::setCircleBoundary(Boolean _circleBoundaryActive) {
    circleBoundaryActive = _circleBoundaryActive;
}
