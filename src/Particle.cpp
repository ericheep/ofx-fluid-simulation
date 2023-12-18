//
//  Particle.cpp
//  fluidSimulation
//

#include "Particle.hpp"

Particle::Particle(ofVec3f _position, float _radius) {
    position = _position;
    predictedPosition = _position;
    radius = _radius;
    lineWidth = 0;
    lineWidthMax = 1;
    magnitude = 0;
    maxMagnitude= 0;
    thickness = 3.0;
    theta = 0;
    velocityHue = 0.0;
    
    velocity = ofVec2f::zero();
    nearDensity = 0.0;
    density = 0.0;
    particleColor = ofColor::black;
}

void Particle::update() {
    magnitude = velocity.length();
    theta = atan(velocity.y / velocity.x);
    float magnitudeScalar = magnitude / 15;
    lineWidth = fmin(magnitudeScalar * lineWidthMax + 2, lineWidthMax);

    float scalar = fmin(velocity.length() / velocityHue, 1.0);
    particleColor = ofColor::blue.getLerped(ofColor::orangeRed, scalar);
    particleColor.setSaturation(175.0);
    // particleColor.a = 100;
}

void Particle::draw() {
    /*
    particleColor.a = 255.0;
    ofSetColor(particleColor);
    ofDrawCircle(position.x, position.y, radius * 0.2);
    particleColor.a = 70.0;
    ofSetColor(particleColor);
    ofDrawCircle(position.x, position.y, radius)
    */
    
    ofSetColor(particleColor);
    ofPushMatrix();
    ofTranslate(position.x, position.y);
    ofRotateRad(theta);
    ofDrawRectangle(-lineWidthMax / 2.0, -thickness / 2.0, lineWidthMax, thickness);
    ofPopMatrix();
}

void Particle::setRadius(float _radius) {
    radius = _radius;
}
