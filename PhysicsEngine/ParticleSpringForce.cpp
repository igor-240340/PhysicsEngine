#include "ParticleSpringForce.h"

ParticleSpringForce::ParticleSpringForce(float restLehgth, float coeff, Particle* other) {
    this->restLength = restLehgth;
    this->coeff = coeff;
    this->other = other;
}

void ParticleSpringForce::ApplyTo(Particle* particle) {
    // ќриентаци€ пружины в пространстве и еЄ текуща€ длина
    // определ€ют направление и величину силы.
    Vec2 springVector = particle->pos - other->pos;
    Vec2 springDir = springVector.Normalized();
    float springLength = springVector.Length();
    float springCompression = springLength - restLength;

    Vec2 force = -(springCompression * coeff) * springDir;
    particle->ApplyForce(force);
}