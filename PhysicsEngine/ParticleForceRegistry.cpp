#include <iostream>

#include "ParticleForceRegistry.h"

void ParticleForceRegistry::Add(Particle* particle, ParticleForce* force) {
    ParticleForcePair pair;
    pair.particle = particle;
    pair.force = force;

    particleForcePairs.push_back(pair);
}

void ParticleForceRegistry::ApplyForces() {
    for (ParticleForcePair& pair : particleForcePairs) {
        pair.force->ApplyTo(pair.particle);
    }
}