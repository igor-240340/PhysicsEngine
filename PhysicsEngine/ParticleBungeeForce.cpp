#include "ParticleBungeeForce.h"

ParticleBungeeForce::ParticleBungeeForce(float restLength, float coeff, Particle* particleB) {
    this->restLength = restLength;
    this->coeff = coeff;
    this->particleB = particleB;
}

void ParticleBungeeForce::ApplyTo(Particle* particleA) {
    Vec2 springVector = particleB->pos - particleA->pos;

    float springCompression = springVector.Length() - restLength;
    // —ила генерируетс€ только когда пружина раст€нута.
    if (springCompression <= 0)
        return;

    Vec2 springDirFromAToB = springVector.Normalized();
    Vec2 force = springDirFromAToB * (springCompression * coeff);
    particleA->ApplyForce(force);
}