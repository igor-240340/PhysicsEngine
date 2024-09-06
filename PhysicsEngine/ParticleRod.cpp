#include "ParticleRod.h"

bool ParticleRod::generate_contact(ParticleContact& contact) const {
    float current_length = get_current_length();
    if (current_length > max_length) {
        contact.particle_a = particle_a;
        contact.particle_b = particle_b;
        contact.restitution = 0.0f;
        contact.penetration = current_length - max_length;
        contact.hit_normal = (particle_b->pos - particle_a->pos).Normalized();
        return true;
    }
    else if (current_length < max_length) {
        contact.particle_a = particle_a;
        contact.particle_b = particle_b;
        contact.restitution = 0.0f;
        contact.penetration = max_length - current_length;
        contact.hit_normal = (particle_a->pos - particle_b->pos).Normalized();
        return true;
    }
    else {
        return false;
    }
}
