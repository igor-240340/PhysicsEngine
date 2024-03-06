#include "ParticleAnchoredSpringForce.h"

ParticleAnchoredSpringForce::ParticleAnchoredSpringForce(float restLength, float coeff, Vec2 anchor){
    this->restLength = restLength;
    this->coeff = coeff;
    this->anchor = anchor;
}

void ParticleAnchoredSpringForce::ApplyTo(Particle* particle) {
    // Вектор из частицы в точку крепления пружины.
    Vec2 springVector = anchor - particle->pos;

    // Если пружина растянута, то компрессия (растяжение) положительна и сила направлена к точке крепления пружины.
    float springCompression = springVector.Length() - restLength;
    Vec2 springDirToAnchor = springVector.Normalized();
    Vec2 force = springDirToAnchor * (springCompression * coeff);
    particle->ApplyForce(force);
}