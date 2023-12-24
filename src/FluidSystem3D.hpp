//
//  FluidSystem3D.hpp
//  fluidSimulation
//

#ifndef FluidSystem3D_hpp
#define FluidSystem3D_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ParticleSystem.hpp"
#include "tbb/parallel_for.h"
#include "tbb/parallel_sort.h"

class FluidSystem3D : public ParticleSystem {
public:
    FluidSystem3D();
    
    void update();

    void resolveCollisions(int particleIndex);
    ofVec3f pushParticlesAwayFromPoint(ofVec3f pointA, ofVec3f pointB);
    ofVec3f pullParticlesToPoint(ofVec3f pointA, ofVec3f pointB);
    
    // math
    float calculatePressureFromDensity(float density);
    float calculateNearPressureFromDensity(float nearDensity);
    float calculateSharedPressure(float densityA, float densityB);
    float calculateSharedNearPressure(float nearDensityA, float nearDensityB);
    
    pair<float, float> calculateDensity(int particleIndex);
    pair<float, float> convertDensityToPressure(float density, float nearDensity);
    ofVec3f calculateViscosityForce(int particleIndex);
    ofVec3f calculatePressureForce(int particleIndex);
    ofVec3f calculateExternalForce(int particleIndex);
    ofVec3f calculateInteractiveForce(int particleIndex);

    // spatial lookup functions
    unsigned int hashCell(ofVec3f cell);
    unsigned int getKeyFromHash(unsigned int hash);
    ofVec3f positionToCellCoordinate(ofVec3f position, float radius);
    vector<int> foreachPointWithinRadius(int particleIndex);
    void updateSpatialLookup();
    
    // reset functions
    void resetRandom();
    void resetGrid(float scale);
    void resetCircle(float scale);

private:
    vector<ofVec3f> cellOffsets;
};

#endif /* FliudSystem3D_hpp */
