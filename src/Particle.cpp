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
    maxSize = 0.0;
    size = 0.0;
    
    circleResolution = 22;
    rectangleResolution = 4;
    
    lerpedMagnitude = 0.0;
    lerpedTheta = 0.0;
    
    initializeCircleMeshes();
    initializeRectangleMeshes();
    initializeVectorMeshes();
    initializeLineMeshes();
    
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
    } else if (_mode == 4) {
        // shapeMode = SVG;
    }
    
}

void Particle::initializeCircleMeshes() {
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

void Particle::initializeRectangleMeshes() {
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

void Particle::initializeVectorMeshes() {
    lineMesh.clear();
    normalizedLineMesh.clear();

    // corner vertices
    vectorMesh.addVertex(ofVec3f(-1, 1, 0));
    vectorMesh.addVertex(ofVec3f(1, 1, 0));
    vectorMesh.addVertex(ofVec3f(1, -1, 0));
    vectorMesh.addVertex(ofVec3f(-1, -1, 0));
    
    normalizedVectorMesh.addVertex(ofVec3f(-1, 1, 0));
    normalizedVectorMesh.addVertex(ofVec3f(1, 1, 0));
    normalizedVectorMesh.addVertex(ofVec3f(1, -1, 0));
    normalizedVectorMesh.addVertex(ofVec3f(-1, -1, 0));
}

void Particle::initializeLineMeshes() {
    lineMesh.clear();
    normalizedLineMesh.clear();

    lineMesh.addVertex(ofVec3f(-1, 0, 0));
    lineMesh.addVertex(ofVec3f(1, 0, 0));
    
    normalizedLineMesh.addVertex(ofVec3f(-1, 0, 0));
    normalizedLineMesh.addVertex(ofVec3f(1, 0, 0));
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
    ofVec3f topLeftOffset = ofVec3f(xOffset, yOffset, zOffset);
    ofVec3f topRightOffset = ofVec3f(-xOffset, -yOffset, zOffset);
    ofVec3f bottomLeftOffset = ofVec3f(xOffset, yOffset, zOffset);
    ofVec3f bottomRightOffset = ofVec3f(-xOffset, -yOffset, zOffset);
    
    float normal = lerpedTheta + HALF_PI;
    float xNormal = cos(normal);
    float yNormal = sin(normal);
    
    ofVec3f n = ofVec3f(xNormal, yNormal, 0) * lineThickness * 0.5;
    
    vectorMesh.setVertex(0, normalizedVectorMesh.getVertex(0) + topLeftOffset + n);
    vectorMesh.setVertex(1, normalizedVectorMesh.getVertex(1) + topRightOffset + n);
    vectorMesh.setVertex(2, normalizedVectorMesh.getVertex(2) + bottomRightOffset - n);
    vectorMesh.setVertex(3, normalizedVectorMesh.getVertex(3) + bottomLeftOffset - n);
}

void Particle::updateLineMesh() {
    ofVec3f p1 = ofVec3f(-xOffset, -yOffset);
    ofVec3f p2 = ofVec3f(xOffset, yOffset);
    
    lineMesh.setVertex(0, normalizedLineMesh.getVertex(0) + p1);
    lineMesh.setVertex(1, normalizedLineMesh.getVertex(1) * p2);
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
            return lineMesh;
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
            setRectangleOffsets();
            updateRectangleMesh();
            break;
        case VECTOR:
            setVectorOffsets();
            updateVectorMesh();
            break;
        case LINE:
            setLineOffsets();
            updateLineMesh();
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

void Particle::setRectangleOffsets() {
    theta = atan(velocity.y / velocity.x);
    lerpedTheta = ofLerp(lerpedTheta, theta, 0.1);
    xOffset = cos(lerpedTheta) * size;
    yOffset = sin(lerpedTheta) * size;
    zOffset = 0;
}

void Particle::setVectorOffsets() {
    theta = atan(velocity.y / velocity.x);
    lerpedTheta = ofLerp(lerpedTheta, theta, 0.1);
    xOffset = cos(lerpedTheta) * size;
    yOffset = sin(lerpedTheta) * size;
    zOffset = 0;
}

void Particle::setLineOffsets() {
    theta = atan(velocity.y / velocity.x);
    lerpedTheta = ofLerp(lerpedTheta, theta, 0.1);
    xOffset = cos(lerpedTheta) * size;
    yOffset = sin(lerpedTheta) * size;
    zOffset = 0;
}

void Particle::setRadius(float _radius) {
    radius = _radius;
}

void Particle::draw() {
    // do nothing
}
