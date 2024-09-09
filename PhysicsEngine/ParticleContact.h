#pragma once

#include "Particle.h"

class ParticleContact
{
public:
    Particle* particle_a;
    Particle* particle_b;

    Vec3 hit_normal;
    Vec3 offset_a;
    Vec3 offset_b;

    float restitution;
    float penetration;

public:
    void resolve();

private:
    void resolve_velocity();
    void resolve_interpenetration();
    float calculate_normal_velocity();
};

