#include "ParticleLink.h"

float ParticleLink::get_current_length() const {
    return (particle_b->pos - particle_a->pos).length();
}
