#include "ParticleSpringForce.h"

ParticleSpringForce::ParticleSpringForce(float restLehgth, float coeff, Particle* particleB) {
    this->restLength = restLehgth;
    this->coeff = coeff;
    this->particleB = particleB;
}

void ParticleSpringForce::ApplyTo(Particle* particleA) {
    Vec3 springVector = particleB->pos - particleA->pos;

    // Если пружина растянута, то компрессия (растяжение) положительна и сила направлена от A к B.
    float springCompression = springVector.length() - restLength;
    Vec3 springDirFromAToB = springVector.normalized();
    Vec3 force = springDirFromAToB * (springCompression * coeff);
    particleA->ApplyForce(force);
}