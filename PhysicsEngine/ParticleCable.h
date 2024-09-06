#pragma once

#include "ParticleLink.h"

class ParticleCable : public ParticleLink
{
public:
    float max_length;
    float restitution;

    bool generate_contact(ParticleContact& contact) const final;
};
