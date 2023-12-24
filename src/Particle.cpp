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
    p1 = ofVec3f::zero();
    p2 = ofVec3f::zero();
    p3 = ofVec3f::zero();
    p4 = ofVec3f::zero();

    
    velocity = ofVec3f::zero();
    nearDensity = 0.0;
    density = 0.0;
    particleColor = ofColor::black;
}

void Particle::update() {
    magnitude = velocity.length();
    theta = atan(velocity.y / velocity.x);
    lineWidth = magnitude * lineWidthScalar + lineWidthMinimum;

    xOffset = cos(theta) * lineWidth;
    yOffset = sin(theta) * lineWidth;
    
    float colorScalar = fmin(magnitude / velocityHue, 1.0);
    particleColor = coolColor.getLerped(hotColor, colorScalar);
    particleColor.setSaturation(175.0);
    particleColor.a = 100;
    setVertices();
}

void Particle::setVertices() {
    float xOffset = cos(theta) * lineWidth / 2.0;
    float yOffset = sin(theta) * lineWidth / 2.0;
    float zOffset = 0;
    
    ofVec3f leftTopFront = ofVec3f(position.x + xOffset, position.y + yOffset, position.z - zOffset);
    ofVec3f leftTopBack = ofVec3f(position.x + xOffset, position.y + yOffset, position.z + zOffset);
    ofVec3f leftBottomFront = ofVec3f(position.x + xOffset, position.y - yOffset, position.z - zOffset);
    ofVec3f leftBottomBack = ofVec3f(position.x + xOffset, position.y - yOffset, position.z + zOffset);
    
    ofVec3f rightTopFront = ofVec3f(position.x - xOffset, position.y + yOffset, position.z - zOffset);
    ofVec3f rightTopBack = ofVec3f(position.x - xOffset, position.y + yOffset, position.z + zOffset);
    ofVec3f rightBottomFront = ofVec3f(position.x - xOffset, position.y - yOffset, position.z - zOffset);
    ofVec3f rightBottomBack = ofVec3f(position.x - xOffset, position.y - yOffset, position.z + zOffset);
    
    mesh.clear();
    
    // left side
    mesh.addVertex(leftTopFront);
    mesh.addVertex(leftTopBack);
    mesh.addVertex(leftBottomFront);
    
    mesh.addVertex(leftTopBack);
    mesh.addVertex(leftBottomBack);
    mesh.addVertex(leftBottomFront);
    
    // top side
    mesh.addVertex(leftTopBack);
    mesh.addVertex(leftTopFront);
    mesh.addVertex(rightTopFront);
    
    mesh.addVertex(rightTopBack);
    mesh.addVertex(rightTopFront);
    mesh.addVertex(leftTopBack);
    
    // bottom side
    mesh.addVertex(leftBottomBack);
    mesh.addVertex(leftBottomFront);
    mesh.addVertex(rightBottomFront);
    
    mesh.addVertex(rightBottomBack);
    mesh.addVertex(rightBottomFront);
    mesh.addVertex(leftBottomBack);
    
    // front side
    mesh.addVertex(leftTopFront);
    mesh.addVertex(leftBottomFront);
    mesh.addVertex(rightTopFront);

    mesh.addVertex(rightTopFront);
    mesh.addVertex(rightBottomFront);
    mesh.addVertex(leftBottomFront);
    
    // back side
    mesh.addVertex(leftTopBack);
    mesh.addVertex(leftBottomBack);
    mesh.addVertex(rightTopBack);

    mesh.addVertex(rightTopBack);
    mesh.addVertex(rightBottomBack);
    mesh.addVertex(leftBottomBack);
    
    // right side
    mesh.addVertex(rightTopFront);
    mesh.addVertex(rightTopBack);
    mesh.addVertex(rightBottomFront);
    
    mesh.addVertex(rightTopBack);
    mesh.addVertex(rightBottomBack);
    mesh.addVertex(rightBottomFront);
}

void Particle::draw() {
    ofSetColor(particleColor);
    mesh.drawFaces();
    // ofDrawLine(position.x - xOffset, position.y - yOffset, position.x + xOffset, position.y + yOffset);
    
}

void Particle::setRadius(float _radius) {
    radius = _radius;
}
