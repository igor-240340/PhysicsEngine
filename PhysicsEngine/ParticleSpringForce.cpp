#include "ParticleSpringForce.h"

ParticleSpringForce::ParticleSpringForce(float restLehgth, float coeff, Particle* particleB) {
    this->restLength = restLehgth;
    this->coeff = coeff;
    this->particleB = particleB;
}

void ParticleSpringForce::ApplyTo(Particle* particleA) {
    Vec2 springVector = particleB->pos - particleA->pos;

    // ≈сли пружина раст€нута, то компресси€ (раст€жение) положительна и сила направлена от A к B.
    float springCompression = springVector.Length() - restLength;
    Vec2 springDirFromAToB = springVector.Normalized();
    Vec2 force = springDirFromAToB * (springCompression * coeff);
    particleA->ApplyForce(force);
}