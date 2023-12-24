//
//  ParticleSystem.hpp
//  fluidSimulation
//

#ifndef ParticleSystem_hpp
#define ParticleSystem_hpp

#include <stdio.h>
#include "Particle.hpp"
#include "Kernels.hpp"

class ParticleSystem {
public:
    ParticleSystem();
    
    vector<Particle> particles;

    float radius, gravity, deltaTime, collisionDamping, predictionFactor, interactiveGravity;
    float targetDensity, nearPressureMultiplier, pressureMultiplier, gravityMultiplier, timeScalar, viscosityStrength;
    int mouseButton, mouseRadius;
    
    ofVec3f down;
    ofVec2f xBounds, yBounds, zBounds, mousePosition;
    ofVec3f boundsSize;
    ofVec3f bounds;
    Kernels kernels;
    Boolean mouseInputActive, pauseActive, nextFrameActive, exportFrameActive, SVG_MODE;

    vector<pair<int, unsigned int>> spatialLookup;
    vector<int> startIndices;
    
    // setters
    void setDeltaTime(float deltaTime);
    void setRadius(float radius);
    void setGravityMultiplier(float gravityMultiplier);
    void setTargetDensity(float targetDensity);
    void setPressureMultiplier(float pressureMultiplier);
    void setNearPressureMultiplier(float nearPressureMultiplier);
    void setViscosityStrength(float viscosityStrength);
    void setCollisionDamping(float collisionDamping);
    void setBoundsSize(ofVec3f bounds);
    void setMouseRadius(int mouseRadius);
    void setLineWidthScalar(float lineWidthMax);
    void setLineWidthMinimum(float lineWidthMinimum);
    void setLineThickness(float lineThickness);
    void setVelocityHue(float hue);
    void setHotColor(ofColor hotColor);
    void setCoolColor(ofColor coolColor);
    void setNumberParticles(int number);
    
    // creation functions
    void addParticle();
    void addParticle(ofVec3f position);
    void addParticle(ofVec3f position, float radius);
    ofVec2f getRandom2DDirection();
    ofVec3f getRandom3DDirection();
    
    // interactions
    void mouseInput(int x, int y);
    void mouseInput(int x, int y, int button, Boolean active);
    
    void draw();
    
    void pause(Boolean pauseButton);
    void nextFrame();
    void saveSvg();
private:
};

#endif /* ParticleSystem_hpp */
