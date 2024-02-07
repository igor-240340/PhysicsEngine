#include "ParticleAnchoredSpringForce.h"

ParticleAnchoredSpringForce::ParticleAnchoredSpringForce(float restLength, float coeff, Vec2 anchor){
    this->restLength = restLength;
    this->coeff = coeff;
    this->anchor = anchor;
}

void ParticleAnchoredSpringForce::ApplyTo(Particle* particle) {
    // ¬ектор из частицы в точку креплени€ пружины.
    Vec2 springVector = anchor - particle->pos;

    // ≈сли пружина раст€нута, то компресси€ (раст€жение) положительна и сила направлена к точке креплени€ пружины.
    float springCompression = springVector.Length() - restLength;
    Vec2 springDirToAnchor = springVector.Normalized();
    Vec2 force = springDirToAnchor * (springCompression * coeff);
    particle->ApplyForce(force);
}