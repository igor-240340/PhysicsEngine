#include "ParticleAnchoredBungeeForce.h"

ParticleAnchoredBungeeForce::ParticleAnchoredBungeeForce(float restLength, float coeff, Vec3 anchor) {
    this->restLength = restLength;
    this->coeff = coeff;
    this->anchor= anchor;
}

void ParticleAnchoredBungeeForce::ApplyTo(Particle* particle) {
    Vec3 springVector = anchor - particle->pos;

    float springCompression = springVector.length() - restLength;
    // Сила генерируется только когда пружина расянута.
    if (springCompression <= 0)
        return;

    Vec3 springDirToAnchor = springVector.normalized();
    Vec3 force = springDirToAnchor * (springCompression * coeff);
    particle->ApplyForce(force);
}
