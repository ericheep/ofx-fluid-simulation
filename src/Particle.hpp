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
    float radius, density, nearDensity, lineWidth, magnitude, maxMagnitude, thickness, theta, lineWidthMax, velocityHue;
    ofVec2f position, velocity, predictedPosition;
    ofColor particleColor;
    vector <int> indicesWithinRadius;
    
    Particle(ofVec3f position, float radius);
    
    void update();
    void draw();
    
    void setRadius(float radius);
private:
};

#endif /* Particle_hpp */
