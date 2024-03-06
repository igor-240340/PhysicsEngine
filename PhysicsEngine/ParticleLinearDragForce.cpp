#include "ParticleLinearDragForce.h"

ParticleLinearDragForce::ParticleLinearDragForce(float coeff) {
    this->coeff = coeff;
}

void ParticleLinearDragForce::ApplyTo(Particle* particle) {
    particle->ApplyForce(-particle->velocity * coeff);
}