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
    // member variables for fluid calculations
    float radius, density, nearDensity;
    
    float lineThickness, lineLength;
    float magnitude, smoothedTheta, minVelocity, maxVelocity;
    float velocityCurve;
    float xOffset, yOffset, zOffset;
    float minSize, maxSize, size;
    ofVec3f position, velocity, predictedPosition, smoothedVelocity;
    vector <ofVec3f> velocities, positions;
    ofColor particleColor, coolColor, hotColor;
    
    ofMesh mesh;
    ofPath poly;
    
    enum MODE { CIRCLE, MESH, LINE, RECTANGLE, TRAIL } mode;

    vector <int> indicesWithinRadius;
    
    Particle(ofVec3f position, float radius);
    
    void update();
    void setMode(int mode);
    void setTrail();
    void setVertices();
    void setSizes();
    void setSmoothedVelocity();
    void setOffsets();
    void draw();
    
    void setRadius(float radius);
private:
};

#endif /* Particle_hpp */
