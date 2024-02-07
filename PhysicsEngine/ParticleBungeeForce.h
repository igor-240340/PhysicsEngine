#pragma once

#include "ParticleForce.h"

class ParticleBungeeForce : public ParticleForce
{
private:
    float coeff = 1.0f;
    float restLength = 1.0f;
    Particle* particleB;

public:
    ParticleBungeeForce(float restLength, float coeff, Particle* particleB);

    void ApplyTo(Particle* particleA) final;
};