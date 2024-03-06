#include "ParticleGravityForce.h"

void ParticleGravityForce::ApplyTo(Particle* particle) {
    particle->ApplyForce(accel * particle->mass);
}