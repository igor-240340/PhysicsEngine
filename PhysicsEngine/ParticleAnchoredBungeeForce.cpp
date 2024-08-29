#include "ParticleAnchoredBungeeForce.h"

ParticleAnchoredBungeeForce::ParticleAnchoredBungeeForce(float restLength, float coeff, Vec2 anchor) {
    this->restLength = restLength;
    this->coeff = coeff;
    this->anchor= anchor;
}

void ParticleAnchoredBungeeForce::ApplyTo(Particle* particle) {
    Vec2 springVector = anchor - particle->pos;

    float springCompression = springVector.Length() - restLength;
    // —ила генерируетс€ только когда пружина раст€нута.
    if (springCompression <= 0)
        return;

    Vec2 springDirToAnchor = springVector.Normalized();
    Vec2 force = springDirToAnchor * (springCompression * coeff);
    particle->ApplyForce(force);
}
