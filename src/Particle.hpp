//
//  Particle.hpp
//  fluidSimulation
//

#ifndef Particle_hpp
#define Particle_hpp

#include <stdio.h>
#include "ofMain.h"

class Particle {
public:
    Particle(ofVec3f position, float radius);

    // member variables for fluid calculations
    float radius, density, nearDensity;
    
    // gui parameters
    float lineThickness, lineLength;
    float minVelocity, maxVelocity, velocityCurve;
    float minSize, maxSize, size;

    // drawing variables
    float magnitude, lerpedMagnitude;
    float theta, lerpedTheta;
    float xOffset, yOffset, zOffset;
    int circleResolution, rectangleResolution;
    
    ofVec3f position, velocity, predictedPosition;
    ofColor particleColor, coolColor, hotColor;
    
    ofMesh circleMesh, rectangleMesh, vectorMesh, shapeMesh;
    ofMesh normalizedCircleMesh, normalizedRectangleMesh;
    
    // drawn mesh
    ofMesh mesh;
    
    enum shapeModes { CIRCLE, RECTANGLE, VECTOR, LINE } shapeMode;

    vector <int> indicesWithinRadius;
    
    ofMesh getShapeMesh();
    void update();
    void setMode(int mode);
    void setTrail();
    void setVertices();
    void setSizes();
    void setSmoothedVelocity();
    void setOffsets();
    void draw();
    void calculateNormalizedCircleMesh(int circleResolution);
    void calculateNormalizedRectangleMesh();
    void calculateNormalizedVectorMesh();
    void updateCircleMesh();
    void updateRectangleMesh();
    void updateVectorMesh();
    
    void setRadius(float radius);
private:
};

#endif /* Particle_hpp */
