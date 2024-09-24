#include "Particle.h"

Particle::Particle() {
    this->pos = Vec3::zero;
    this->velocity = Vec3::zero;

    this->mass = 1.0f;
    this->inv_mass = 1.0f / mass;
}

Particle::Particle(Vec3 pos, Vec3 velocity, float mass) {
    this->pos = pos;
    this->velocity = velocity;

    this->mass = mass;
    this->inv_mass = 1.0f / mass;
}

void Particle::ApplyForce(Vec3 force) {
    net_force += force;
}