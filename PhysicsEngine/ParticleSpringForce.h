#pragma once

#include "ParticleForce.h"

class ParticleSpringForce : public ParticleForce
{
private:
    float coeff = 1.0f;
    float restLength = 1.0f;
    Particle* particleB;

public:
    ParticleSpringForce(float restLength, float coeff, Particle* particleB);

    void ApplyTo(Particle* particleA) final;
};