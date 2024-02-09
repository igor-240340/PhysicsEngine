#pragma once

#include "ParticleForce.h"

class ParticleBuoyantForce : public ParticleForce
{
private:
    static const float GravityAccelY;

    float liquidSurfaceY;
    float particleSize;     // ��� ������� ������������� ���� ������� ��������������� ��� ��� � ��������� ��������.
    float particleHalfSize;
    float particleVolume;
    float liquidDensity;
    float hydroDragCoeff;

public:
    ParticleBuoyantForce(float liquidSurfaceY, float particleSize, float liquidDensity, float hydroDragCoeff);

    void ApplyTo(Particle* particle) final;
};