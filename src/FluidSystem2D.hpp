//
//  FluidSystem2D.hpp
//  fluidSimulation
//

#ifndef FluidSystem2D_hpp
#define FluidSystem2D_hpp

#include <stdio.h>
#include "ofMain.h"
#include "Particle.hpp"
#include "Kernels.hpp"
#include "tbb/parallel_for.h"
#include "tbb/parallel_sort.h"

class FluidSystem2D {
public:
    FluidSystem2D();
    
    void update();
    void draw();
    
    // creation functions
    void addParticle();
    void addParticle(ofVec2f position);
    void addParticle(ofVec2f position, float radius);
    void setNumberParticles(int number);
    ofVec2f getRandomDirection();
    
    // interactions
    void mouseInput(int x, int y);
    void mouseInput(int x, int y, int button, Boolean active);
    void resolveCollisions(int particleIndex);
    ofVec2f pushParticlesAwayFromPoint(ofVec2f pointA, ofVec2f pointB);
    ofVec2f pullParticlesToPoint(ofVec2f pointA, ofVec2f pointB);
    
    // math
    float calculatePressureFromDensity(float density);
    float calculateNearPressureFromDensity(float nearDensity);
    float calculateSharedPressure(float densityA, float densityB);
    float calculateSharedNearPressure(float nearDensityA, float nearDensityB);
    
    pair<float, float> calculateDensity(int particleIndex);
    pair<float, float> convertDensityToPressure(float density, float nearDensity);
    ofVec2f calculateViscosityForce(int particleIndex);
    ofVec2f calculatePressureForce(int particleIndex);
    ofVec2f calculateExternalForce(int particleIndex);
    ofVec2f calculateInteractiveForce(int particleIndex);
    
    // setters
    void setDeltaTime(float deltaTime);
    void setRadius(float radius);
    void setGravityMultiplier(float gravityMultiplier);
    void setTargetDensity(float targetDensity);
    void setPressureMultiplier(float pressureMultiplier);
    void setNearPressureMultiplier(float nearPressureMultiplier);
    void setViscosityStrength(float viscosityStrength);
    void setCollisionDamping(float collisionDamping);
    void setBoundingBox(ofVec2f bounds);
    void setMouseRadius(int mouseRadius);
    void setLineWidthScalar(float lineWidthMax);
    void setLineWidthMinimum(float lineWidthMinimum);
    void setLineThickness(float lineThickness);
    void setVelocityHue(float hue);
    void setHotColor(ofColor hotColor);
    void setCoolColor(ofColor coolColor);

    // spatial lookup functions
    unsigned int hashCell(int cellX, int cellY);
    unsigned int getKeyFromHash(unsigned int hash);
    pair<int, int> positionToCellCoordinate(ofVec2f position, float radius);
    vector<int> foreachPointWithinRadius(int particleIndex);
    void updateSpatialLookup();
    
    // reset functions
    void resetRandom();
    void resetGrid(float scale);
    void resetCircle(float scale);
    void pause(Boolean pauseButton);
    void nextFrame();
    void saveSvg();

private:
    Kernels kernels;
    vector<Particle> particles;
    
    float radius, gravity, deltaTime, collisionDamping, predictionFactor, interactiveGravity;
    float targetDensity, nearPressureMultiplier, pressureMultiplier, gravityMultiplier, timeScalar, viscosityStrength;
    ofVec2f down, xBounds, yBounds, mousePosition;
    ofRectangle boundingBox;
    vector<pair<int, unsigned int>> spatialLookup;
    vector<int> startIndices;
    vector<pair<int, int>> cellOffsets;
    Boolean mouseInputActive, pauseActive, nextFrameActive, exportFrameActive, SVG_MODE;
    int mouseButton, mouseRadius;
};

#endif /* ParticleSystem_hpp */
