#pragma once

#include <list>

#include "Particle.h"
#include "ParticleForce.h"

class ParticleForceRegistry
{
private:
    struct ParticleForcePair
    {
        Particle* particle;
        ParticleForce* force;
    };

    std::list<ParticleForcePair> particleForcePairs;

public:
    void Add(Particle* particle, ParticleForce* force);
    void Remove(Particle* particle, ParticleForce* force);
    void Clear();
    void ApplyForces();
};