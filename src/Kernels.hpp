//
//  Kernels.hpp
//  fluidSimulation
//

#ifndef Kernels_hpp
#define Kernels_hpp

#include <stdio.h>
#include "ofMain.h"

class Kernels {
public:
    Kernels();
    
    void calculateVolumesFromRadius(float radius);
    
    float smoothingKernelPoly6(float distance, float radius);
    float spikyKernelPow3(float distance, float radius);
    float spikyKernelPow2(float distance, float radius);
    float derivativeSpikyPow3(float distance, float radius);
    float derivativeSpikyPow2(float distance, float radius);
    
    float densityKernel(float distance, float radius);
    float nearDensityKernel(float distnace, float radius);
    float smoothinkgKernel(float distance, float radius);
    float viscosityKernel(float distance, float radius);
    float densityDerivative(float distance, float radius);
    float nearDensityDerivative(float distance, float radius);
    
private:
    float poly6ScalingFactor;
    float spikyPow3ScalingFactor;
    float spikyPow2ScalingFactor;
    float spikyPow3DerivativeScalingFactor;
    float spikyPow2DerivativeScalingFactor;
};

#endif /* Kernels_hpp */
