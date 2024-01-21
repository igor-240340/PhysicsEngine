#include "Particle.h"

Particle::Particle(Vec2 pos, Vec2 velocity, float mass) {
    this->pos = pos;
    this->velocity = velocity;

    this->mass = mass;
    this->invMass = 1.0f / mass;
}

void Particle::ApplyForce(Vec2 force) {
    netForce += force;
}