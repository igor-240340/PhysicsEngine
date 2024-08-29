#pragma once

#include "ParticleForce.h"

class ParticleAnchoredBungeeForce : public ParticleForce
{
private:
    float coeff = 1.0f;
    float restLength = 1.0f;
    Vec2 anchor;

public:
    ParticleAnchoredBungeeForce(float restLength, float coeff, Vec2 anchor);

    void ApplyTo(Particle* particle) final;
};
