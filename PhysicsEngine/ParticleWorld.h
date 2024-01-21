#pragma once

#include <list>

#include "Particle.h"
#include "ParticleForceRegistry.h"

class ParticleWorld
{
private:
    float size; // Размеры мира.
    std::list<Particle*> particles;

public:
    ParticleForceRegistry forceRegistry;

public:
    void AddParticle(Particle* particle);
    const std::list<Particle*>& Particles();
    void Step(float dt);
};