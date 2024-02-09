#include <iostream>

#include "ParticleBuoyantForce.h"

const float ParticleBuoyantForce::GravityAccelY = -9.8f;

ParticleBuoyantForce::ParticleBuoyantForce(float liquidSurfaceY, float particleSize, float liquidDensity, float hydroDragCoeff) {
    this->liquidSurfaceY = liquidSurfaceY;
    this->particleSize = particleSize;
    this->liquidDensity = liquidDensity;
    this->hydroDragCoeff = hydroDragCoeff;

    particleHalfSize = particleSize / 2.0f;
    particleVolume = particleSize * particleSize * particleSize;
}

// ������ ������� ���������� � ������� ���������� ����. �������������, ���������� � ����� �������.
// �������, �������� ����� ������� �������, �� ��� ����� ��������� ��������� ��������� �������.
// NOTE: ��� ������������ ���������� � �������� �� ��� Y.
void ParticleBuoyantForce::ApplyTo(Particle* particle) {
    // ���������� Y ������ ����� �������.
    float particleBottomY = particle->pos.y - particleHalfSize;

    // ������ ���������� - ��� ���������� �� ������ ����� ������� �� ����������� ����.
    float submersionHeight = liquidSurfaceY - particleBottomY;

    // ������� �� ��������� � ��������.
    if (submersionHeight <= 0.0f)
        return;

    // ����� ������� ��������� ��������� � ��������, ����� ����������� �������� ����� ������� ������ �������.
    float displacedLiquidVolume = particleVolume;

    // ����� ������� ��������� �� ���������, ����� ����������� �������� ����� ������ ����������� �����.
    // 
    // NOTE: � ����� ������ ������ ����� ������� ����������� ���������� ����� � ������ (��������� - ����� �������������� ������).
    // ��� ��������, ���, ��������, ���� ����� N ��������� �� ������ ������, �� �����, ��������������� ���� ������,
    // ����� ���������� �� ������� ������ �� �� N ���������.
    // 
    // �������������, ���� ������� ����������� ������, �� ����� ����� ������� ������������ ������.
    if (submersionHeight < particleSize)
        displacedLiquidVolume = particleVolume * (submersionHeight / particleSize);

    // �� �����������, ������������� ���� �� ������ ����� ���� ����������� �������� � ���������� �����.
    float displacedLiquidWeightY = (displacedLiquidVolume * liquidDensity) * GravityAccelY;
    Vec2 buoyantForce(0.0f, -displacedLiquidWeightY);
    particle->ApplyForce(buoyantForce);
    
    std::cout << "buoyantForce.y: " << buoyantForce.y << std::endl;

    Vec2 hydroDragForce = -particle->velocity * hydroDragCoeff;
    particle->ApplyForce(hydroDragForce);

    std::cout << "hydroDragForce.y: " << hydroDragForce.y << std::endl;
}