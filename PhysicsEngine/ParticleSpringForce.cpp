#include <iostream>

#include "ParticleSpringForce.h"

ParticleSpringForce::ParticleSpringForce(float restLehgth, float coeff, Particle* other) {
    this->restLength = restLehgth;
    this->coeff = coeff;
    this->other = other;
}

void ParticleSpringForce::ApplyTo(Particle* particle) {
    Vec2 force;

    // ќриентаци€ пружины в пространстве и еЄ текуща€ длина
    // определ€ют направление и величину силы.
    Vec2 springVector = particle->pos - other->pos;
    Vec2 springDir = springVector.Normalized();
    float springLength = springVector.Length();
    float springCompression = springLength - restLength;

    force = -(springCompression * coeff) * springDir;
    particle->ApplyForce(force);

    std::cout << "spring force: " << force.y << std::endl;
}