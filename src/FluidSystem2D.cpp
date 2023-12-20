//
//  FluidSystem2D.cpp
//  fluidSimulation
//

#include "FluidSystem2D.hpp"

FluidSystem2D::FluidSystem2D() {
    radius = 10;
    kernels.calculateVolumesFromRadius(radius);
    
    predictionFactor = 1.0f / 120.0f;
    pressureMultiplier = 1.0;
    nearPressureMultiplier = 1.0;
    targetDensity = 1.0;
    viscosityStrength = 0.5;
    
    // 9.8 meters per second
    gravity = 9.8;
    gravityMultiplier = 1.0;
    collisionDamping = 0.26;
    pauseActive = false;
    
    cellOffsets.push_back(pair<int, int>(-1, 1));
    cellOffsets.push_back(pair<int, int>(-1, 0));
    cellOffsets.push_back(pair<int, int>(-1, -1));
    cellOffsets.push_back(pair<int, int>(0, 1));
    cellOffsets.push_back(pair<int, int>(0, 0));
    cellOffsets.push_back(pair<int, int>(0, -1));
    cellOffsets.push_back(pair<int, int>(1, 1));
    cellOffsets.push_back(pair<int, int>(1, 0));
    cellOffsets.push_back(pair<int, int>(1, -1));
    
    down = ofVec2f(0.0, 1.0);
    setBoundingBox(ofVec2f(ofGetWidth() * 0.8, ofGetHeight() * 0.8));
}

void FluidSystem2D::draw() {
    if (exportFrameActive) ofBeginSaveScreenAsSVG("export.svg");
    
    ofFill();
    for (int i = 0; i < particles.size(); i++) {
        particles[i].draw();
    }
    
    if (exportFrameActive) {
        ofEndSaveScreenAsSVG();
        exportFrameActive = false;
    }
}

void FluidSystem2D::update() {
    if (!pauseActive || nextFrameActive) {
        tbb::parallel_for( tbb::blocked_range<int>(0, particles.size()), [&](tbb::blocked_range<int> r) {
            for (int i = r.begin(); i < r.end(); i++) {
                ofVec2f externalForce = calculateExternalForce(i);
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
                ofVec2f pressureForce = calculatePressureForce(i);
                ofVec2f pressureAcceleration = pressureForce / particles[i].density;
                particles[i].velocity += pressureAcceleration * deltaTime;
                
                ofVec2f viscosityForce = calculateViscosityForce(i);
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

ofVec2f FluidSystem2D::calculateInteractiveForce(int particleIndex) {
    ofVec2f particlePosition = particles[particleIndex].position;
    ofVec2f interactiveForce= ofVec2f::zero();
    
    if (mouseButton == 0) {
        interactiveForce = pushParticlesAwayFromPoint(mousePosition, particlePosition);
    }
    if (mouseButton == 2) {
        interactiveForce = pullParticlesToPoint(mousePosition, particlePosition);
    }
    
    return interactiveForce;
}

ofVec2f FluidSystem2D::pullParticlesToPoint(ofVec2f pointA, ofVec2f pointB) {
    ofVec2f interactiveForce = ofVec2f::zero();
    
    float inputRadius = mouseRadius;
    float squareDistance = pointA.squareDistance(pointB);
    
    if (squareDistance < inputRadius * inputRadius) {
        float distance = sqrt(squareDistance);
        ofVec2f direction = (pointA - pointB) / distance;
        float scalarProximity = 1.0 - distance / inputRadius;
        
        interactiveForce =  direction * 15.0f * scalarProximity;
    }
    return interactiveForce;
}

ofVec2f FluidSystem2D::pushParticlesAwayFromPoint(ofVec2f pointA, ofVec2f pointB) {
    ofVec2f interactiveForce = ofVec2f::zero();
    
    float inputRadius = mouseRadius;
    float squareDistance = pointA.squareDistance(pointB);
    
    if (squareDistance < inputRadius * inputRadius) {
        float distance = sqrt(squareDistance);
        ofVec2f direction = (pointB - pointA) / distance;
        float scalarProximity = 1.0 - distance / inputRadius;
        
        interactiveForce =  direction * 50.0f * scalarProximity * scalarProximity;
    }
    return interactiveForce;
}

ofVec2f FluidSystem2D::calculateExternalForce(int particleIndex) {
    ofVec2f interactiveForce = ofVec2f::zero();
    
    if (mouseInputActive) {
        interactiveForce = calculateInteractiveForce(particleIndex);
    }
    
    return interactiveForce + down * gravity * gravityMultiplier * deltaTime;
}

pair<float, float> FluidSystem2D::calculateDensity(int particleIndex) {
    vector<int> indicesWithinRadius = particles[particleIndex].indicesWithinRadius;
    ofVec2f particlePosition = particles[particleIndex].predictedPosition;
    
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

ofVec2f FluidSystem2D::calculatePressureForce(int particleIndex) {
    vector<int> indicesWithinRadius = particles[particleIndex].indicesWithinRadius;
    ofVec2f particlePosition = particles[particleIndex].predictedPosition;
    float density = particles[particleIndex].density;
    float nearDensity = particles[particleIndex].nearDensity;
    float pressure = calculatePressureFromDensity(density);
    float nearPressure = calculateNearPressureFromDensity(nearDensity);
    
    ofVec2f pressureForce = ofVec2f::zero();
    
    for (int i = 0; i < indicesWithinRadius.size(); i++) {
        int neighborParticleIndex = indicesWithinRadius[i];
        if (particleIndex == neighborParticleIndex) continue;
        
        ofVec2f neighborPosition = particles[neighborParticleIndex].predictedPosition;
        float distance = particlePosition.distance(neighborPosition);
        ofVec2f direction = (neighborPosition - particlePosition) / distance;
        direction = distance == 0.0 ? getRandomDirection() : direction;
        
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

ofVec2f FluidSystem2D::calculateViscosityForce(int particleIndex) {
    vector<int> indicesWithinRadius = particles[particleIndex].indicesWithinRadius;
    ofVec2f particlePosition = particles[particleIndex].predictedPosition;
    ofVec2f viscosityForce = ofVec2f::zero();
    
    for (int i = 0; i < indicesWithinRadius.size(); i++) {
        int neighborParticleIndex = indicesWithinRadius[i];
        if (particleIndex == neighborParticleIndex) continue;
        
        float distance = particlePosition.distance(particles[neighborParticleIndex].predictedPosition);
        float influence = kernels.viscosityKernel(distance, radius);
        viscosityForce += (particles[neighborParticleIndex].velocity - particles[particleIndex].velocity) * influence;
    }
    
    return viscosityForce * viscosityStrength;
}

float FluidSystem2D::calculatePressureFromDensity(float density) {
    float densityError = density - targetDensity;
    return densityError * pressureMultiplier;
}

float FluidSystem2D::calculateNearPressureFromDensity(float nearDensity) {
    return nearDensity * nearPressureMultiplier;
}

// mouse input

void FluidSystem2D::mouseInput(int x, int y, int button, Boolean active) {
    mouseInputActive = active;
    mouseButton = button;
    mouseInput(x, y);
}

void FluidSystem2D::mouseInput(int x, int y) {
    mousePosition = ofVec2f(x, y);
}

// spatial lookup

vector<int> FluidSystem2D::foreachPointWithinRadius(int particleIndex) {
    ofVec2f position = particles[particleIndex].position;
    
    pair<int, int> center = positionToCellCoordinate(position, radius);
    int centerX = center.first;
    int centerY = center.second;
    float squareRadius = radius * radius;
    
    vector<int> indicesWithinRadius;
    
    for (auto offsetPair : cellOffsets) {
        int offsetX = offsetPair.first;
        int offsetY = offsetPair.second;
        
        unsigned int key = getKeyFromHash(hashCell(centerX + offsetX, centerY + offsetY));
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

void FluidSystem2D::updateSpatialLookup() {
    tbb::parallel_for( tbb::blocked_range<int>(0, particles.size()), [&](tbb::blocked_range<int> r) {
        for (int i = r.begin(); i < r.end(); i++) {
            pair<int, int> cell = positionToCellCoordinate(particles[i].position, radius);
            unsigned int cellKey = getKeyFromHash(hashCell(cell.first, cell.second));
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

unsigned int FluidSystem2D::hashCell(int cellX, int cellY) {
    unsigned int a = u_int(cellX * 15823);
    unsigned int b = u_int(cellY * 9737333);
    return a + b;
}

unsigned int FluidSystem2D::getKeyFromHash(unsigned int hash) {
    return hash % u_int(spatialLookup.size());
}

pair<int, int> FluidSystem2D::positionToCellCoordinate(ofVec2f position, float radius) {
    return pair<int, int> (int(position.x / radius), int(position.y / radius));
}

void FluidSystem2D::resolveCollisions(int particleIndex) {
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
}

// create particles

void FluidSystem2D::addParticle() {
    float x = ofRandom(boundingBox.x, boundingBox.x + boundingBox.width);
    float y = ofRandom(boundingBox.y, boundingBox.y + boundingBox.height);
    addParticle(ofVec2f(x, y));
}

void FluidSystem2D::addParticle(ofVec2f position) {
    particles.push_back(Particle(position, radius));
    spatialLookup.resize(particles.size());
    startIndices.resize(particles.size());
}

ofVec2f FluidSystem2D::getRandomDirection() {
    ofVec2f randomDirection = ofVec2f(1.0, 0.0).rotateRad(ofRandom(0, TWO_PI));
    return randomDirection;
}

// reset particles

void FluidSystem2D::resetRandom() {
    for (int i = 0; i < particles.size(); i++) {
        float x = ofRandom(boundingBox.x, boundingBox.x + boundingBox.width);
        float y = ofRandom(boundingBox.y, boundingBox.y + boundingBox.height);
        particles[i].position = ofVec2f(x, y);
        particles[i].velocity = getRandomDirection();
    }
}

void FluidSystem2D::resetGrid(float scale) {
    int rows = ceil(pow(particles.size(), 0.5));
    int cols = ceil(pow(particles.size(), 0.5));
    
    float xOffset = ofGetWidth() / 2.0 - boundingBox.width / 2.0 * scale;
    float yOffset = ofGetHeight() / 2.0 - boundingBox.height / 2.0 * scale;
    
    for (int i = 0; i < rows; i++) {
        float xSpace = boundingBox.width * scale / float(rows + 1);
        float x = xSpace * (i + 1) + xOffset;
        
        for (int j = 0; j < cols; j++) {
            int particleIndex = i * cols + j;
            if (particleIndex >= particles.size()) return;
            
            float ySpace = boundingBox.height * scale / float(cols + 1);
            float y = ySpace * (j + 1) + yOffset;
            
            float jitterX = xSpace * ofRandom(-0.1, 0.1);
            float jitterY = ySpace * ofRandom(-0.1, 0.1);
            
            particles[particleIndex].position = ofVec2f(x + jitterX, y + jitterY);
            particles[particleIndex].velocity = getRandomDirection();
        }
    }
}

void FluidSystem2D::resetCircle(float scale) {
    ofVec2f center = ofVec2f(ofGetWidth() / 2.0, ofGetHeight() / 2.0);
    
    float diameter = boundingBox.width;
    if (boundingBox.height < boundingBox.width) {
        diameter = boundingBox.height;
    }
    
    float radius = diameter / 2.0 * scale;
    
    for (int i = 0; i < particles.size(); i++) {
        float theta = ofRandom(0, TWO_PI);
        float magnitude = ofRandom(0, radius);
        
        float x = cos(theta) * magnitude;
        float y = sin(theta) * magnitude;
        
        particles[i].position = ofVec2f(x, y) + center;
        particles[i].velocity = getRandomDirection();
    }
}

void FluidSystem2D::pause(Boolean _pauseActive) {
    pauseActive = _pauseActive;
}

void FluidSystem2D::nextFrame() {
    nextFrameActive = true;
}

void FluidSystem2D::saveSvg() {
    exportFrameActive = true;
}

// setters

void FluidSystem2D::setNumberParticles(int number) {
    if (number > particles.size()) {
        while (particles.size() < number) {
            addParticle();
        }
        spatialLookup.resize(particles.size());
        startIndices.resize(particles.size());
    }
    if (number < particles.size()) {
        while (particles.size() > number) {
            particles.pop_back();
        }
        spatialLookup.resize(particles.size());
        startIndices.resize(particles.size());
    }
}

void FluidSystem2D::setBoundingBox(ofVec2f _bounds) {
    boundingBox.setFromCenter(ofGetWidth() / 2.0, ofGetHeight() / 2.0, _bounds.x, _bounds.y);
    xBounds = ofVec2f(boundingBox.x + radius, boundingBox.x + boundingBox.width - radius);
    yBounds = ofVec2f(boundingBox.y + radius, boundingBox.y + boundingBox.height - radius);
}

void FluidSystem2D::setRadius(float _radius) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].setRadius(_radius);
    }
    
    if (radius != _radius) {
        kernels.calculateVolumesFromRadius(_radius);
        radius = _radius;
    }
}

void FluidSystem2D::setGravityMultiplier(float _gravityMultiplier) {
    gravityMultiplier = _gravityMultiplier;
}

void FluidSystem2D::setDeltaTime(float _deltaTime) {
    deltaTime = _deltaTime;
}

void FluidSystem2D::setCollisionDamping(float _collisionDamping) {
    collisionDamping = _collisionDamping;
}

void FluidSystem2D::setTargetDensity(float _targetDensity) {
    targetDensity = _targetDensity;
}

void FluidSystem2D::setPressureMultiplier(float _pressureMultiplier) {
    pressureMultiplier = _pressureMultiplier;
}

void FluidSystem2D::setViscosityStrength(float _viscosityStrength) {
    viscosityStrength = _viscosityStrength;
}

void FluidSystem2D::setNearPressureMultiplier(float _nearPressureMultiplier) {
    nearPressureMultiplier = _nearPressureMultiplier;
}

void FluidSystem2D::setMouseRadius(int _mouseRadius) {
    mouseRadius = _mouseRadius;
}

void FluidSystem2D::setVelocityHue(float _velocityHue) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].velocityHue = _velocityHue;
    }
}

void FluidSystem2D::setCoolColor(ofColor coolColor) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].coolColor = coolColor;
    }
}

void FluidSystem2D::setHotColor(ofColor hotColor) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].hotColor = hotColor;
    }
}

void FluidSystem2D::setLineWidthScalar(float lineWidthScalar) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].lineWidthScalar = lineWidthScalar;
    }
}

void FluidSystem2D::setLineWidthMinimum(float lineWidthMinimum) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].lineWidthMinimum = lineWidthMinimum;
    }
}

void FluidSystem2D::setLineThickness(float lineThickness) {
    for (int i = 0; i < particles.size(); i++) {
        particles[i].lineThickness = lineThickness;
    }
}
