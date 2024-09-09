#include "ParticleCable.h"

bool ParticleCable::generate_contact(ParticleContact& contact) const {
    float current_length = get_current_length();
    if (current_length >= max_length) {
        contact.particle_a = particle_a;
        contact.particle_b = particle_b;
        contact.restitution = restitution;
        contact.penetration = current_length - max_length;
        contact.hit_normal = (particle_b->pos - particle_a->pos).normalized();
        
        return true;
    }
    else {
        return false;
    }
}
