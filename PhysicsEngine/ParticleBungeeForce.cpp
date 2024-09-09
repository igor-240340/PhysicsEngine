#include "ParticleBungeeForce.h"

ParticleBungeeForce::ParticleBungeeForce(float restLength, float coeff, Particle* particleB) {
    this->restLength = restLength;
    this->coeff = coeff;
    this->particleB = particleB;
}

void ParticleBungeeForce::ApplyTo(Particle* particleA) {
    Vec3 springVector = particleB->pos - particleA->pos;

    float springCompression = springVector.length() - restLength;
    // Сила генерируется только когда пружина растянута.
    if (springCompression <= 0)
        return;

    Vec3 springDirFromAToB = springVector.normalized();
    Vec3 force = springDirFromAToB * (springCompression * coeff);
    particleA->ApplyForce(force);
}