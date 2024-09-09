#include "Particle.h"

Particle::Particle(Vec3 pos, Vec3 velocity, float mass) {
    this->pos = pos;
    this->velocity = velocity;

    this->mass = mass;
    this->invMass = 1.0f / mass;
}

void Particle::ApplyForce(Vec3 force) {
    netForce += force;
}