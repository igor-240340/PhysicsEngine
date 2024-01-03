#pragma once

#include <list>

#include "Particle.h"

class ParticleWorld
{
private:
    float size; // Размеры мира.
    std::list<Particle> particles;

public:
    void AddParticle(Particle& particle);
    const std::list<Particle>& Particles();
    void Step(float dt);
};