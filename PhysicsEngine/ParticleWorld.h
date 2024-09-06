#pragma once

#include <list>

#include "Particle.h"
#include "ParticleForceRegistry.h"
#include "ParticleContactGenerator.h"
#include "ParticleContactResolver.h"

class ParticleWorld
{
private:
    std::list<Particle*> particles;
    std::list<ParticleContactGenerator*> contact_generators;
    ParticleContactResolver contact_resolver;

public:
    ParticleForceRegistry force_registry;

public:
    void add_particle(Particle* particle);
    void add_contact_generator(ParticleContactGenerator* contactGenerator);

    const std::list<Particle*>& Particles();
    void Step(float dt);
};