#include <iostream>

#include "ParticleWorld.h"

void ParticleWorld::add_particle(Particle* particle) {
    particles.push_back(particle);
}

void ParticleWorld::add_contact_generator(ParticleContactGenerator* contactGenerator) {
    contact_generators.push_back(contactGenerator);
}

const std::vector<Particle*>& ParticleWorld::get_particles() {
    return particles;
}

void ParticleWorld::step(float dt) {
    // Применяем к массам, связанные с ними генераторы сил.
    force_registry.ApplyForces();

    // Интегрируем ускорение и скорость.
    for (Particle* p : particles) {
        if (p->inv_mass == 0.0f)
            continue;

        p->pos += p->velocity * dt;

        std::cout << "===\n";
        std::cout << "p->netForce.y: " << p->net_force.y << std::endl;
        std::cout << "===\n";

        Vec3 accel = p->net_force * p->inv_mass;
        p->velocity += accel * dt;

        p->net_force = Vec3::zero;
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