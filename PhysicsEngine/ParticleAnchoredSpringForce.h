#pragma once

#include "ParticleForce.h"

class ParticleAnchoredSpringForce : public ParticleForce
{
private:
    float coeff = 1.0f;
    float restLength = 1.0f;
    Vec3 anchor;

public:
    ParticleAnchoredSpringForce(float restLength, float coeff, Vec3 anchor);

    void ApplyTo(Particle* particle) final;
};