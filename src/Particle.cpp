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

    velocity = ofVec3f::zero();
    nearDensity = 0.0;
    density = 0.0;
    particleColor = ofColor::black;
    
    minVelocity = 0.0;
    maxVelocity = 1.0;
    
    minSize = 0.0;
    maxSize = 100.0;
    size = 0.0;
    
    circleResolution = 22;
    rectangleResolution = 4;
    
    lerpedMagnitude = 0.0;
    lerpedTheta = 0.0;
    
    calculateNormalizedCircleMesh(circleResolution);
    calculateNormalizedRectangleMesh();
    calculateNormalizedVectorMesh();
    
    shapeMode = CIRCLE;
}

void Particle::setMode(int _mode) {
    if (_mode == 0) {
        shapeMode = CIRCLE;
    } else if (_mode == 1) {
        shapeMode = RECTANGLE;
    } else if (_mode == 2) {
        shapeMode = VECTOR;
    } else if (_mode == 3) {
        shapeMode = LINE;
    }
}

void Particle::calculateNormalizedCircleMesh(int circleResolution) {
    circleMesh.clear();
    normalizedCircleMesh.clear();
    float deltaTheta = TWO_PI / float(circleResolution);
    
    for (float i = 0; i < TWO_PI; i = i + deltaTheta) {
        float x = cos(i);
        float y = sin(i);
        circleMesh.addVertex(ofVec3f(x, y, 0));
        normalizedCircleMesh.addVertex(ofVec3f(x, y, 0));
    }
}

void Particle::calculateNormalizedRectangleMesh() {
    rectangleMesh.clear();
    normalizedRectangleMesh.clear();

    // cornver vertices
    rectangleMesh.addVertex(ofVec3f(-1, 1, 0));
    rectangleMesh.addVertex(ofVec3f(1, 1, 0));
    rectangleMesh.addVertex(ofVec3f(1, -1, 0));
    rectangleMesh.addVertex(ofVec3f(-1, -1, 0));
    
    normalizedRectangleMesh.addVertex(ofVec3f(-1, 1, 0));
    normalizedRectangleMesh.addVertex(ofVec3f(1, 1, 0));
    normalizedRectangleMesh.addVertex(ofVec3f(1, -1, 0));
    normalizedRectangleMesh.addVertex(ofVec3f(-1, -1, 0));
}

void Particle::calculateNormalizedVectorMesh() {
    vectorMesh.clear();

    // cornver vertices
    vectorMesh.addVertex(ofVec3f(-1, 1, 0));
    vectorMesh.addVertex(ofVec3f(1, 1, 0));
    vectorMesh.addVertex(ofVec3f(1, -1, 0));
    vectorMesh.addVertex(ofVec3f(-1, -1, 0));
}

void Particle::updateCircleMesh() {
    for (int i = 0; i < circleMesh.getNumVertices(); i++) {
        circleMesh.setVertex(i, normalizedCircleMesh.getVertex(i) * size);
    }
}

void Particle::updateRectangleMesh() {
    ofVec3f topLeftOffset = ofVec3f(-xOffset, yOffset, 0);
    ofVec3f topRightOffset = ofVec3f(xOffset, yOffset, 0);
    ofVec3f bottomRightOffset = ofVec3f(xOffset, -yOffset, 0);
    ofVec3f bottomLeftOffset = ofVec3f(-xOffset, -yOffset, 0);

    rectangleMesh.setVertex(0, normalizedRectangleMesh.getVertex(0) + topLeftOffset);
    rectangleMesh.setVertex(1, normalizedRectangleMesh.getVertex(1) + topRightOffset);
    rectangleMesh.setVertex(2, normalizedRectangleMesh.getVertex(2) + bottomRightOffset);
    rectangleMesh.setVertex(3, normalizedRectangleMesh.getVertex(3) + bottomLeftOffset);
}

void Particle::updateVectorMesh() {
    ofVec3f topLeftOffset = ofVec3f(xOffset, yOffset + lineThickness, zOffset);
    ofVec3f topRightOffset = ofVec3f(-xOffset, -yOffset + lineThickness, zOffset);
    ofVec3f bottomLeftOffset = ofVec3f(xOffset, yOffset - lineThickness, zOffset);
    ofVec3f bottomRightOffset = ofVec3f(-xOffset, -yOffset - lineThickness, zOffset);
    
    vectorMesh.setVertex(0, normalizedRectangleMesh.getVertex(0) + topLeftOffset);
    vectorMesh.setVertex(1, normalizedRectangleMesh.getVertex(1) + topRightOffset);
    vectorMesh.setVertex(2, normalizedRectangleMesh.getVertex(2) + bottomRightOffset);
    vectorMesh.setVertex(3, normalizedRectangleMesh.getVertex(3) + bottomLeftOffset);
}

ofMesh Particle::getShapeMesh() {
    switch (shapeMode) {
        case CIRCLE:
            return circleMesh;
            break;
        case RECTANGLE:
            return rectangleMesh;
            break;
        case VECTOR:
            return vectorMesh;
            break;
        case LINE:
            return rectangleMesh;
            break;
    }
}

void Particle::update() {
    setSizes();
    
    switch (shapeMode) {
        case CIRCLE:
            updateCircleMesh();
            break;
        case RECTANGLE:
            setOffsets();
            updateRectangleMesh();
            break;
        case VECTOR:
            setOffsets();
            updateVectorMesh();
            break;
        case LINE:
            setOffsets();
            // setVertices();
            updateRectangleMesh();
            break;
    }
}

void Particle::setSizes() {
    lerpedMagnitude = ofLerp(lerpedMagnitude, velocity.length(), 0.1);
    
    // clip, scale, and curve
    float clippedMagnitude = std::max(minVelocity, std::min(lerpedMagnitude, maxVelocity));
    float scaledMagnitude = ofMap(clippedMagnitude, minVelocity, maxVelocity, 0.0, 1.0);
    float curvedMagnitude = pow(scaledMagnitude, velocityCurve);
    
    particleColor = coolColor.getLerped(hotColor, curvedMagnitude);
    size = minSize + (maxSize - minSize) * curvedMagnitude;
}

void Particle::setOffsets() {
    theta = atan(velocity.y / velocity.x);
    lerpedTheta = ofLerp(lerpedTheta, theta, 0.1);
    xOffset = cos(lerpedTheta) * size;
    yOffset = sin(lerpedTheta) * size;
    zOffset = 0;
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
    // do nothing
}

void Particle::setRadius(float _radius) {
    radius = _radius;
}
