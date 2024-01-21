#pragma once

#include "ParticleForce.h"

// ��������� ����, ������� ���������� ��������� � �����, ����� ������� �� ��������� ���������.
class ParticleGravityForce : public ParticleForce
{
private:
    Vec2 accel = Vec2(0.0f, -9.8f);

public:
    void ApplyTo(Particle* particle) final;
};