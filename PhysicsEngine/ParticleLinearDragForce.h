#pragma once

#include "ParticleForce.h"

class ParticleLinearDragForce : public ParticleForce
{
private:
    float coeff = 1.0f;

public:
    ParticleLinearDragForce(float coeff);

    void ApplyTo(Particle* particle) final;
};