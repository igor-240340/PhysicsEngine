#include <iostream>

#include "ParticleWorld.h"

void ParticleWorld::add_particle(Particle* particle) {
    particles.push_back(particle);
}

void ParticleWorld::add_contact_generator(ParticleContactGenerator* contactGenerator) {
    contact_generators.push_back(contactGenerator);
}

const std::list<Particle*>& ParticleWorld::Particles() {
    return particles;
}

void ParticleWorld::Step(float dt) {
    // Применяем к массам, связанные с ними генераторы сил.
    force_registry.ApplyForces();

    // Интегрируем ускорение и скорость.
    for (Particle* p : particles) {
        if (p->invMass == 0.0f)
            continue;

        p->pos += p->velocity * dt;

        std::cout << "===\n";
        std::cout << "p->netForce.y: " << p->netForce.y << std::endl;
        std::cout << "===\n";

        Vec2 accel = p->netForce * p->invMass;
        p->velocity += accel * dt;

        p->netForce = Vec2::Zero;
    }

    // Формируем список масс, находящихся в состоянии удара.
    std::list<ParticleContact*> contacts;
    ParticleContact tmp_contact;
    for (ParticleContactGenerator* contact_generator : contact_generators) {
        if (contact_generator->generate_contact(tmp_contact)) {
            ParticleContact* contact = new ParticleContact();
            *contact = tmp_contact;
            contacts.push_back(contact);
        }
    }

    contact_resolver.resolve_contacts(contacts);
}