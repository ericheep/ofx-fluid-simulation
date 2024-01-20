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

    float radius, gravityConstant, deltaTime, collisionDamping, predictionFactor, interactiveGravity;
    float targetDensity, nearPressureMultiplier, pressureMultiplier, gravityMultiplier, timeScalar, viscosityStrength;
    int mouseButton, mouseRadius;
    
    ofVboMesh mesh;
    
    float centerX, centerY;
    ofVec2f gravityForce;
    ofVec2f center;
    ofVec2f xBounds, yBounds, zBounds, mousePosition;
    ofVec3f boundsSize;
    ofVec3f bounds;
    Kernels kernels;
    Boolean mouseInputActive, pauseActive, nextFrameActive, exportFrameActive, SVG_MODE;
    Boolean circleBoundaryActive;

    vector<pair<int, unsigned int>> spatialLookup;
    vector<int> startIndices;
    
    // setters
    void setDeltaTime(float deltaTime);
    void setRadius(float radius);
    void setGravityMultiplier(float gravityMultiplier);
    void setGravityRotation(ofVec2f gravityRotation);
    void setTargetDensity(float targetDensity);
    void setPressureMultiplier(float pressureMultiplier);
    void setNearPressureMultiplier(float nearPressureMultiplier);
    void setViscosityStrength(float viscosityStrength);
    void setCollisionDamping(float collisionDamping);
    void setBoundsSize(ofVec3f bounds);
    void setMouseRadius(int mouseRadius);
    void setMinVelocity(float minVelocity);
    void setMaxVelocity(float maxVelocity);
    void setMinSize(float minSize);
    void setMaxSize(float maxSize);
    void setVelocityCurve(float velocityCurve);
    void setLineThickness(float lineThickness);
    void setVelocityHue(float hue);
    void setHotColor(ofColor hotColor);
    void setCoolColor(ofColor coolColor);
    void setNumberParticles(int number);
    void setLineCurve(float lineCurve);
    void setCenter(float centerX, float centerY);
    void setMode(int mode);
    void setCircleBoundary(Boolean circleBoundaryActive);
    
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
