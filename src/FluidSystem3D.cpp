//
//  FluidSystem3D.cpp
//  fluidSimulation
//

#include "FluidSystem3D.hpp"

FluidSystem3D::FluidSystem3D() {
    kernels.calculate3DVolumesFromRadius(radius);
    
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            for (int k = -1; k < 2; k++) {
                cellOffsets.push_back(ofVec3f(i, j, k));
            }
        }
    }
}

void FluidSystem3D::update() {
    if (!pauseActive || nextFrameActive) {
        tbb::parallel_for( tbb::blocked_range<int>(0, particles.size()), [&](tbb::blocked_range<int> r) {
            for (int i = r.begin(); i < r.end(); i++) {
                ofVec3f externalForce = calculateExternalForce(i);
                particles[i].velocity += externalForce;
                particles[i].predictedPosition = particles[i].position + particles[i].velocity * predictionFactor;
            }
        });
        
        updateSpatialLookup();
        
        tbb::parallel_for( tbb::blocked_range<int>(0, particles.size()), [&](tbb::blocked_range<int> r) {
            for (int i = r.begin(); i < r.end(); i++) {
                particles[i].indicesWithinRadius = foreachPointWithinRadius(i);
                pair<float, float> densities = calculateDensity(i);
                particles[i].density = densities.first;
                particles[i].nearDensity = densities.second;
            }
        });
        
        tbb::parallel_for( tbb::blocked_range<int>(0, particles.size()), [&](tbb::blocked_range<int> r) {
            for (int i = r.begin(); i < r.end(); i++) {
                ofVec3f pressureForce = calculatePressureForce(i);
                ofVec3f pressureAcceleration = pressureForce / particles[i].density;
                particles[i].velocity += pressureAcceleration * deltaTime;
                
                ofVec3f viscosityForce = calculateViscosityForce(i);
                particles[i].velocity += viscosityForce * deltaTime;
            }
        });
        
        tbb::parallel_for( tbb::blocked_range<int>(0, particles.size()), [&](tbb::blocked_range<int> r) {
            for (int i = r.begin(); i < r.end(); i++) {
                particles[i].position += particles[i].velocity * deltaTime;
                resolveCollisions(i);
            }
        });
        
        nextFrameActive = false;
    }
    
    tbb::parallel_for( tbb::blocked_range<int>(0, particles.size()), [&](tbb::blocked_range<int> r) {
        for (int i = r.begin(); i < r.end(); i++) {
            particles[i].update();
        }
    });
}

ofVec3f FluidSystem3D::calculateInteractiveForce(int particleIndex) {
    ofVec3f particlePosition = particles[particleIndex].position;
    ofVec3f interactiveForce= ofVec3f::zero();
    
    if (mouseButton == 0) {
        interactiveForce = pushParticlesAwayFromPoint(mousePosition, particlePosition);
    }
    if (mouseButton == 2) {
        interactiveForce = pullParticlesToPoint(mousePosition, particlePosition);
    }
    
    return interactiveForce;
}

ofVec3f FluidSystem3D::pullParticlesToPoint(ofVec3f pointA, ofVec3f pointB) {
    ofVec3f interactiveForce = ofVec3f::zero();
    
    float inputRadius = mouseRadius;
    float squareDistance = pointA.squareDistance(pointB);
    
    if (squareDistance < inputRadius * inputRadius) {
        float distance = sqrt(squareDistance);
        ofVec3f direction = (pointA - pointB) / distance;
        float scalarProximity = 1.0 - distance / inputRadius;
        
        interactiveForce =  direction * 15.0f * scalarProximity;
    }
    return interactiveForce;
}

ofVec3f FluidSystem3D::pushParticlesAwayFromPoint(ofVec3f pointA, ofVec3f pointB) {
    ofVec3f interactiveForce = ofVec3f::zero();
    
    float inputRadius = mouseRadius;
    float squareDistance = pointA.squareDistance(pointB);
    
    if (squareDistance < inputRadius * inputRadius) {
        float distance = sqrt(squareDistance);
        ofVec3f direction = (pointB - pointA) / distance;
        float scalarProximity = 1.0 - distance / inputRadius;
        
        interactiveForce =  direction * 50.0f * scalarProximity * scalarProximity;
    }
    return interactiveForce;
}

ofVec3f FluidSystem3D::calculateExternalForce(int particleIndex) {
    ofVec3f interactiveForce = ofVec3f::zero();
    
    if (mouseInputActive) {
        interactiveForce = calculateInteractiveForce(particleIndex);
    }
    
    return interactiveForce + gravityForce * gravityMultiplier * deltaTime;
}

pair<float, float> FluidSystem3D::calculateDensity(int particleIndex) {
    vector<int> indicesWithinRadius = particles[particleIndex].indicesWithinRadius;
    ofVec3f particlePosition = particles[particleIndex].predictedPosition;
    
    float density = 0.0f;
    float nearDensity = 0.0f;
    
    for (int i = 0; i < indicesWithinRadius.size(); i++) {
        int neighborParticleIndex = indicesWithinRadius[i];
        
        float distance = particlePosition.distance(particles[neighborParticleIndex].predictedPosition);
        density += kernels.densityKernel(distance, radius);
        nearDensity += kernels.nearDensityKernel(distance, radius);
    }
    
    return pair<float, float> (density, nearDensity);
}

ofVec3f FluidSystem3D::calculatePressureForce(int particleIndex) {
    vector<int> indicesWithinRadius = particles[particleIndex].indicesWithinRadius;
    ofVec3f particlePosition = particles[particleIndex].predictedPosition;
    float density = particles[particleIndex].density;
    float nearDensity = particles[particleIndex].nearDensity;
    float pressure = calculatePressureFromDensity(density);
    float nearPressure = calculateNearPressureFromDensity(nearDensity);
    
    ofVec3f pressureForce = ofVec3f::zero();
    
    for (int i = 0; i < indicesWithinRadius.size(); i++) {
        int neighborParticleIndex = indicesWithinRadius[i];
        if (particleIndex == neighborParticleIndex) continue;
        
        ofVec3f neighborPosition = particles[neighborParticleIndex].predictedPosition;
        float distance = particlePosition.distance(neighborPosition);
        ofVec3f direction = (neighborPosition - particlePosition) / distance;
        direction = distance == 0.0 ? getRandom3DDirection() : direction;
        
        float slope = kernels.densityDerivative(distance, radius);
        float nearSlope = kernels.nearDensityDerivative(distance, radius);
        
        float neighborDensity = particles[neighborParticleIndex].density;
        float neighborNearDensity = particles[neighborParticleIndex].nearDensity;
        float neighborPressure = calculatePressureFromDensity(neighborDensity);
        float neighborNearPressure = calculateNearPressureFromDensity(neighborNearDensity);
        
        float sharedPressure = (pressure + neighborPressure) * 0.5;
        float sharedNearPressure = (nearPressure = neighborNearPressure) * 0.5;
        
        pressureForce += sharedPressure * direction * slope / density;
        pressureForce += sharedNearPressure * direction * nearSlope / nearDensity;
    }
    
    return pressureForce;
}

ofVec3f FluidSystem3D::calculateViscosityForce(int particleIndex) {
    vector<int> indicesWithinRadius = particles[particleIndex].indicesWithinRadius;
    ofVec3f particlePosition = particles[particleIndex].predictedPosition;
    ofVec3f viscosityForce = ofVec3f::zero();
    
    for (int i = 0; i < indicesWithinRadius.size(); i++) {
        int neighborParticleIndex = indicesWithinRadius[i];
        if (particleIndex == neighborParticleIndex) continue;
        
        float distance = particlePosition.distance(particles[neighborParticleIndex].predictedPosition);
        float influence = kernels.viscosityKernel(distance, radius);
        viscosityForce += (particles[neighborParticleIndex].velocity - particles[particleIndex].velocity) * influence;
    }
    
    return viscosityForce * viscosityStrength;
}

float FluidSystem3D::calculatePressureFromDensity(float density) {
    float densityError = density - targetDensity;
    return densityError * pressureMultiplier;
}

float FluidSystem3D::calculateNearPressureFromDensity(float nearDensity) {
    return nearDensity * nearPressureMultiplier;
}

// spatial lookup

vector<int> FluidSystem3D::foreachPointWithinRadius(int particleIndex) {
    ofVec3f position = particles[particleIndex].position;
    
    ofVec3f center = positionToCellCoordinate(position, radius);
    int centerX = center.x;
    int centerY = center.y;
    int centerZ = center.z;
    float squareRadius = radius * radius;
    
    vector<int> indicesWithinRadius;
    
    for (auto offsetPair : cellOffsets) {
        int offsetX = offsetPair.x;
        int offsetY = offsetPair.y;
        int offsetZ = offsetPair.z;
        
        ofVec3f cell = ofVec3f(centerX + offsetX, centerY + offsetY, centerX + offsetZ);
        unsigned int key = getKeyFromHash(hashCell(cell));
        int cellStartIndex = startIndices[key];
        
        for (int i = cellStartIndex; i < spatialLookup.size(); i++) {
            if (spatialLookup[i].second != key) break;
            
            int otherParticleIndex = spatialLookup[i].first;
            float squareDistance = particles[otherParticleIndex].position.squareDistance(position);
            
            if (squareDistance <= squareRadius) {
                indicesWithinRadius.push_back(otherParticleIndex);
            }
        }
    }
    
    return indicesWithinRadius;
}

void FluidSystem3D::updateSpatialLookup() {
    tbb::parallel_for( tbb::blocked_range<int>(0, particles.size()), [&](tbb::blocked_range<int> r) {
        for (int i = r.begin(); i < r.end(); i++) {
            ofVec3f cell = positionToCellCoordinate(particles[i].position, radius);
            unsigned int cellKey = getKeyFromHash(hashCell(cell));
            spatialLookup[i] = pair<int, unsigned int> (i, cellKey);
            startIndices[i] = INT_MAX;
        }
    });
    
    tbb::parallel_sort(spatialLookup.begin(), spatialLookup.end(), [](auto &left, auto &right) {
        return left.second < right.second;
    });
    
    tbb::parallel_for( tbb::blocked_range<int>(0, particles.size()), [&](tbb::blocked_range<int> r) {
        for (int i = r.begin(); i < r.end(); i++) {
            unsigned int key = spatialLookup[i].second;
            unsigned int keyPrev = i == 0 ? UINT_MAX : spatialLookup[i - 1].second;
            if (key != keyPrev) {
                startIndices[key] = i;
            }
        }
    });
}

unsigned int FluidSystem3D::hashCell(ofVec3f cell) {
    unsigned int a = u_int(cell.x * 15823);
    unsigned int b = u_int(cell.y * 9737333);
    unsigned int c = u_int(cell.z * 440817757);
    
    return a + b + c;
}

unsigned int FluidSystem3D::getKeyFromHash(unsigned int hash) {
    return hash % u_int(spatialLookup.size());
}

ofVec3f FluidSystem3D::positionToCellCoordinate(ofVec3f position, float radius) {
    return ofVec3f(int(position.x / radius), int(position.y / radius), int(position.z / radius));
}

void FluidSystem3D::resolveCollisions(int particleIndex) {
    if (particles[particleIndex].position.x < xBounds.x) {
        particles[particleIndex].velocity.x *= -1.0 * collisionDamping;
        particles[particleIndex].position.x = xBounds.x;
    }
    
    if (particles[particleIndex].position.x > xBounds.y) {
        particles[particleIndex].velocity.x *= -1.0 * collisionDamping;
        particles[particleIndex].position.x = xBounds.y;
    }
    
    if (particles[particleIndex].position.y < yBounds.x) {
        particles[particleIndex].velocity.y *= -1.0 * collisionDamping;
        particles[particleIndex].position.y = yBounds.x;
    }
    
    if (particles[particleIndex].position.y > yBounds.y) {
        particles[particleIndex].velocity.y *= -1.0 * collisionDamping;
        particles[particleIndex].position.y = yBounds.y;
    }
    
    if (particles[particleIndex].position.z < zBounds.x) {
        particles[particleIndex].velocity.z *= -1.0 * collisionDamping;
        particles[particleIndex].position.z = zBounds.x;
    }
    
    if (particles[particleIndex].position.z > zBounds.y) {
        particles[particleIndex].velocity.z *= -1.0 * collisionDamping;
        particles[particleIndex].position.z = zBounds.y;
    }
}

// reset particles

void FluidSystem3D::resetRandom() {
    for (int i = 0; i < particles.size(); i++) {
        float x = ofRandom(bounds.x, bounds.x + boundsSize.x);
        float y = ofRandom(bounds.y, bounds.y + boundsSize.y);
        float z = ofRandom(bounds.z, bounds.z + boundsSize.z);
        
        particles[i].position = ofVec3f(x, y, z);
        particles[i].velocity = getRandom3DDirection();
    }
}

// ya this is is a fun one to figure out
void FluidSystem3D::resetGrid(float scale) {
    int rows = ceil(pow(particles.size(), 0.5));
    int cols = ceil(pow(particles.size(), 0.5));
    
    float width = boundsSize.x;
    float height = boundsSize.y;
    
    float xOffset = ofGetWidth() / 2.0 - width / 2.0 * scale;
    float yOffset = ofGetHeight() / 2.0 - height / 2.0 * scale;
    
    for (int i = 0; i < rows; i++) {
        float xSpace = width * scale / float(rows + 1);
        float x = xSpace * (i + 1) + xOffset;
        
        for (int j = 0; j < cols; j++) {
            int particleIndex = i * cols + j;
            if (particleIndex >= particles.size()) return;
            
            float ySpace = height * scale / float(cols + 1);
            float y = ySpace * (j + 1) + yOffset;
            
            float jitterX = xSpace * ofRandom(-0.1, 0.1);
            float jitterY = ySpace * ofRandom(-0.1, 0.1);
            
            particles[particleIndex].position = ofVec2f(x + jitterX, y + jitterY);
            particles[particleIndex].velocity = getRandom2DDirection();
        }
    }
}

// generate points within sphere, from this beautiful website
// https://karthikkaranth.me/blog/generating-random-points-in-a-sphere/
void FluidSystem3D::resetCircle(float scale) {
    ofVec3f center = ofVec3f(ofGetWidth() / 2.0, ofGetHeight() / 2.0, ofGetWidth() / 2.0);
    
    float diameter = boundsSize.x;
    if (boundsSize.y < boundsSize.x) {
        diameter = boundsSize.y;
    }
    
    float radius = diameter / 2.0 * scale;
    
    for (int i = 0; i < particles.size(); i++) {
        float u = ofRandom(0.0, 1.0);
        float v = ofRandom(0.0, 1.0);
        float theta = u * TWO_PI;
        float phi = acos(2.0 * v - 1.0);
        float r = pow(ofRandom(0.0, 1.0), 0.333) * radius;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);
        float x = r * sinPhi * cosTheta;
        float y = r * sinPhi * sinTheta;
        float z = r * cosPhi;
        
        particles[i].position = ofVec3f(x, y, z) + center;
        particles[i].velocity = getRandom3DDirection();
    }
}
