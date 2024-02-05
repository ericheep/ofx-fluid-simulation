//
//  FluidSystem2D.hpp
//  fluidSimulation
//

#ifndef FluidSystem2D_hpp
#define FluidSystem2D_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ParticleSystem.hpp"
#include "tbb/parallel_for.h"
#include "tbb/parallel_sort.h"

class FluidSystem2D : public ParticleSystem {
public:
    FluidSystem2D();
    
    void update();

    void resolveCollisions(int particleIndex);
    ofVec2f pushParticlesAwayFromPoint(ofVec2f pointA, ofVec2f pointB, ofVec2f velocity);
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

private:
    vector<ofVec2f> cellOffsets;
};

#endif /* FluidSystem2D_hpp */
