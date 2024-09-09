#pragma once

#include "ParticleForce.h"

class ParticleAnchoredBungeeForce : public ParticleForce
{
private:
    float coeff = 1.0f;
    float restLength = 1.0f;
    Vec3 anchor;

public:
    ParticleAnchoredBungeeForce(float restLength, float coeff, Vec3 anchor);

    void ApplyTo(Particle* particle) final;
};
