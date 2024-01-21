#include "ParticleWorld.h"

void ParticleWorld::AddParticle(Particle* particle) {
    particles.push_back(particle);
}

const std::list<Particle*>& ParticleWorld::Particles() {
    return particles;
}

void ParticleWorld::Step(float dt) {
    forceRegistry.ApplyForces();

    for (Particle* p : particles) {
        p->pos += p->velocity * dt;

        Vec2 accel = p->netForce * p->invMass;
        p->velocity += accel * dt;

        p->netForce = Vec2::Zero;
    }
}