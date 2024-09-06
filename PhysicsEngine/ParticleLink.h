#pragma once

#include "ParticleContactGenerator.h"

class ParticleLink : public ParticleContactGenerator
{
public:
    Particle* particle_a;
    Particle* particle_b;

protected:
    float get_current_length() const;

public:
    virtual bool generate_contact(ParticleContact& contact) const = 0;
};
