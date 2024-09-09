#pragma once

#include "ParticleForce.h"

// Вычисляет силу, которую необходимо приложить к массе, чтобы придать ей указанное ускорение.
class ParticleGravityForce : public ParticleForce
{
private:
    Vec3 accel = Vec3(0.0f, -9.8f, 0.0f);

public:
    void ApplyTo(Particle* particle) final;
};