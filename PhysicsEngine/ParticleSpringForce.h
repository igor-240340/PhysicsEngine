#pragma once

#include "ParticleForce.h"

class ParticleSpringForce : public ParticleForce
{
private:
    float coeff = 1.0f;
    float restLength = 1.0f;
    Particle* other;

public:
    ParticleSpringForce(float restLength, float coeff, Particle* other);

    void ApplyTo(Particle* particle) final;
};