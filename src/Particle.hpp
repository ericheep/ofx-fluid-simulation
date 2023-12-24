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
    float radius, density, nearDensity, lineWidth, magnitude, maxMagnitude, theta, lineWidthScalar, lineWidthMinimum, velocityHue, lineThickness;
    float xOffset, yOffset;
    ofVec3f position, velocity, predictedPosition;
    ofColor particleColor, coolColor, hotColor;
    
    // drawing points
    ofVec3f p1, p2, p3, p4;
    
    ofMesh mesh;

    vector <int> indicesWithinRadius;
    
    Particle(ofVec3f position, float radius);
    
    void update();
    void setVertices();
    void draw();
    
    void setRadius(float radius);
private:
};

#endif /* Particle_hpp */
