#pragma once

#include "Particle.h"

class ParticleForce
{
public:
    virtual void ApplyTo(Particle* particle) = 0;
};