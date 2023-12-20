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
    lineWidthScalar = 1;
    lineThickness = 1;
    magnitude = 0;
    maxMagnitude= 0;
    theta = 0;
    velocityHue = 0.0;
    
    velocity = ofVec3f::zero();
    nearDensity = 0.0;
    density = 0.0;
    particleColor = ofColor::black;
}

void Particle::update() {
    magnitude = velocity.length();
    theta = atan(velocity.y / velocity.x);
    
    lineWidth = magnitude * lineWidthScalar + lineWidthMinimum;
    
    float colorScalar = fmin(magnitude / velocityHue, 1.0);
    particleColor = coolColor.getLerped(hotColor, colorScalar);
    // particleColor.setSaturation(175.0);
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
    
    ofPushMatrix();
    ofTranslate(position.x, position.y);
    ofRotateRad(theta);
    ofSetColor(particleColor);
    if (lineThickness == 1) {
        ofDrawLine(-lineWidth / 2.0, 0, lineWidth, 0);
    } else {
        ofDrawRectangle(-lineWidth / 2.0, -lineThickness / 2.0, lineWidth, lineThickness);
    }
    ofPopMatrix();
}

void Particle::setRadius(float _radius) {
    radius = _radius;
}
