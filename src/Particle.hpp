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
    
    ofMesh circleMesh, rectangleMesh, vectorMesh, lineMesh;
    ofMesh normalizedCircleMesh, normalizedRectangleMesh;
    ofMesh normalizedLineMesh, normalizedVectorMesh;
    
    // drawn mesh
    ofMesh mesh;
    
    enum shapeModes { CIRCLE, RECTANGLE, VECTOR, LINE } shapeMode;

    vector <int> indicesWithinRadius;
    
    ofMesh getShapeMesh();
    
    void setMode(int mode);
    void setRadius(float radius);
    void setVertices();
    void setSizes();
    void setSmoothedVelocity();
    void setRectangleOffsets();
    void setVectorOffsets();
    void setLineOffsets();

    void initializeCircleMeshes();
    void initializeRectangleMeshes();
    void initializeVectorMeshes();
    void initializeLineMeshes();

    void updateCircleMesh();
    void updateRectangleMesh();
    void updateVectorMesh();
    void updateLineMesh();
    
    void update();
    void draw();
private:
};

#endif /* Particle_hpp */
