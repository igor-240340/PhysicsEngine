#pragma once

#include "ParticleForce.h"

// ¬ычисл€ет силу, которую необходимо приложить к массе, чтобы придать ей указанное ускорение.
class ParticleGravityForce : public ParticleForce
{
private:
    Vec2 accel = Vec2(0.0f, -9.8f);

public:
    void ApplyTo(Particle* particle) final;
};