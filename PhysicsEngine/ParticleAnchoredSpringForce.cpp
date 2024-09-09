#include "ParticleAnchoredSpringForce.h"

ParticleAnchoredSpringForce::ParticleAnchoredSpringForce(float restLength, float coeff, Vec3 anchor){
    this->restLength = restLength;
    this->coeff = coeff;
    this->anchor = anchor;
}

void ParticleAnchoredSpringForce::ApplyTo(Particle* particle) {
    // Вектор из частицы в точку крепления пружины.
    Vec3 springVector = anchor - particle->pos;

    // Если пружина растянута, то компрессия (растяжение) положительна и сила направлена к точке крепления пружины.
    float springCompression = springVector.length() - restLength;
    Vec3 springDirToAnchor = springVector.normalized();
    Vec3 force = springDirToAnchor * (springCompression * coeff);
    particle->ApplyForce(force);
}