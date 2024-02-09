#pragma once

#include "ParticleForce.h"

class ParticleBuoyantForce : public ParticleForce
{
private:
    static const float GravityAccelY;

    float liquidSurfaceY;
    float particleSize;     // При расчете выталкивающей силы частица рассматривается как куб с ненулевой стороной.
    float particleHalfSize;
    float particleVolume;
    float liquidDensity;
    float hydroDragCoeff;

public:
    ParticleBuoyantForce(float liquidSurfaceY, float particleSize, float liquidDensity, float hydroDragCoeff);

    void ApplyTo(Particle* particle) final;
};