//
//  Particle.cpp
//  fluidSimulation
//

#include "Particle.hpp"

Particle::Particle(ofVec3f _position, float _radius) {
    position = _position;
    predictedPosition = _position;
    radius = _radius;
    lineThickness = 1;
    magnitude = 0;

    mode = CIRCLE;
    velocity = ofVec3f::zero();
    nearDensity = 0.0;
    density = 0.0;
    particleColor = ofColor::black;
    
    minVelocity = 0.0;
    maxVelocity = 1.0;
    
    minSize = 0.0;
    maxSize = 100.0;
}

void Particle::setMode(int _mode) {
    if (_mode == 0) {
        mode = CIRCLE;
    } else if (_mode == 1) {
        mode = MESH;
    } else if (_mode == 2) {
        mode = LINE;
    } else if (_mode == 3) {
        mode = RECTANGLE;
    } else if (_mode == 4) {
        mode = TRAIL;
    }
}

void Particle::update() {
    setSmoothedVelocity();
    setSizes();
    
    switch (mode) {
        case CIRCLE:
            break;
        case MESH:
            setOffsets();
            setVertices();
            break;
        case RECTANGLE:
            setOffsets();
            break;
        case LINE:
            setOffsets();
            break;
        case TRAIL:
            setTrail();
            break;
    }
}

void Particle::setTrail() {
    while (positions.size() < 10) {
        positions.push_back(position);
    }
    
    positions.erase(positions.begin(), positions.begin() + 1);
}

void Particle::setSizes() {
    magnitude = smoothedVelocity.length();

    // clip!
    float clippedMagnitude = std::max(minVelocity, std::min(magnitude, maxVelocity));
    float scaledMagnitude = ofMap(clippedMagnitude, minVelocity, maxVelocity, 0.0, 1.0);
    float curvedMagnitude = pow(scaledMagnitude, velocityCurve);
    
    particleColor = coolColor.getLerped(hotColor, curvedMagnitude);
    size = minSize + (maxSize - minSize) * curvedMagnitude;
}

void Particle::setOffsets() {
    smoothedTheta = atan(smoothedVelocity.y / smoothedVelocity.x);
    xOffset = cos(smoothedTheta) * size / 2.0;
    yOffset = sin(smoothedTheta) * size / 2.0;
    zOffset = 0;
}

void Particle:: setSmoothedVelocity() {
    while (velocities.size() < 10) {
        velocities.push_back(velocity);
    }
    
    ofVec3f sumVelocity = ofVec3f::zero();
    for (int i = 0; i < velocities.size(); i++) {
        sumVelocity += velocities[i];
    }
    smoothedVelocity = sumVelocity / velocities.size();
    velocities.erase(velocities.begin(), velocities.begin() + 1);
}

void Particle::setVertices() {
    float yThickness = lineThickness / 2.0;
    
    ofVec3f leftTopFront = ofVec3f(position.x + xOffset, position.y + yOffset + yThickness, position.z - zOffset);
    ofVec3f leftTopBack = ofVec3f(position.x + xOffset, position.y + yOffset + yThickness, position.z + zOffset);
    ofVec3f leftBottomFront = ofVec3f(position.x + xOffset, position.y + yOffset - yThickness, position.z - zOffset);
    ofVec3f leftBottomBack = ofVec3f(position.x + xOffset, position.y + yOffset - yThickness, position.z + zOffset);
    
    ofVec3f rightTopFront = ofVec3f(position.x - xOffset, position.y - yOffset + yThickness, position.z - zOffset);
    ofVec3f rightTopBack = ofVec3f(position.x - xOffset, position.y - yOffset + yThickness, position.z + zOffset);
    ofVec3f rightBottomFront = ofVec3f(position.x - xOffset, position.y - yOffset - yThickness, position.z - zOffset);
    ofVec3f rightBottomBack = ofVec3f(position.x - xOffset, position.y - yOffset - yThickness, position.z + zOffset);
    
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
    
    switch (mode) {
        case CIRCLE:
            ofDrawCircle(position.x, position.y, size * 0.5);
            break;
        case MESH:
            mesh.drawFaces();
            break;
        case RECTANGLE:
            ofDrawRectangle(position.x - xOffset * 0.5, position.y - yOffset * 0.5, xOffset, yOffset);
            break;
        case LINE:
            ofDrawLine(position.x - xOffset, position.y - yOffset, position.x + xOffset, position.y + yOffset);
            break;
        case TRAIL:
            for (int i = 0; i < positions.size(); i++) {
                ofDrawCircle(position.x, position.y, size * 0.5);
            }
            
            break;
    }
}

void Particle::setRadius(float _radius) {
    radius = _radius;
}
