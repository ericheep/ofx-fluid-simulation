//
//  Kernels.cpp
//  fluidSimulation
//

#include "Kernels.hpp"

Kernels::Kernels() {
    
}

void Kernels::calculate3DVolumesFromRadius(float radius) {
    poly6ScalingFactor = 315 / (64 * PI * pow(abs(radius), 9));
    spikyPow3ScalingFactor = 15 / (PI * pow(radius, 6));
    spikyPow2ScalingFactor = 15 / (2 * PI * pow(radius, 5));
    spikyPow3DerivativeScalingFactor = 45 / (pow(radius, 6) * PI);
    spikyPow2DerivativeScalingFactor = 15 / (pow(radius, 5) * PI);
}

void Kernels::calculate2DVolumesFromRadius(float radius) {
    poly6ScalingFactor = 4.0 / (PI * pow(radius, 8.0));
    spikyPow3ScalingFactor = 10 / (PI * pow(radius, 5.0));
    spikyPow2ScalingFactor = 6.0 / (PI * pow(radius, 4.0));
    spikyPow3DerivativeScalingFactor = 30.0 / (PI * pow(radius, 5.0));
    spikyPow2DerivativeScalingFactor = 12.0 / (PI * pow(radius, 4.0));
}

float Kernels::smoothingKernelPoly6(float distance, float radius) {
    if (distance > radius) return 0;
    
    float value = radius * radius - distance * distance;
    return value * value * value * poly6ScalingFactor;
}

float Kernels::spikyKernelPow3(float distance, float radius) {
    if (distance > radius) return 0;
    
    float value = radius - distance;
    return value * value * value * spikyPow3ScalingFactor;
}

float Kernels::spikyKernelPow2(float distance, float radius) {
    if (distance > radius) return 0;
    
    float value = radius - distance;
    return value * value * spikyPow2ScalingFactor;
}

float Kernels::derivativeSpikyPow3(float distance, float radius) {
    if (distance >= radius) return 0;
    
    float value = radius - distance;
    return -value * value * spikyPow3DerivativeScalingFactor;
}

float Kernels::derivativeSpikyPow2(float distance, float radius) {
    if (distance >= radius) return 0;
    
    float value = radius - distance;
    return value * spikyPow2DerivativeScalingFactor;
}

float Kernels::densityKernel(float distance, float radius) {
    return spikyKernelPow2(distance, radius);
}

float Kernels::nearDensityKernel(float distance, float radius) {
    return spikyKernelPow3(distance, radius);
}

float Kernels::viscosityKernel(float distance, float radius) {
    return smoothingKernelPoly6(distance, radius);
}

float Kernels::densityDerivative(float distance, float radius) {
    return derivativeSpikyPow2(distance, radius);
}

float Kernels::nearDensityDerivative(float distance, float radius) {
    return derivativeSpikyPow3(distance, radius);
}
