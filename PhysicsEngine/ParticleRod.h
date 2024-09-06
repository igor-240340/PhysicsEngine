#pragma once

#include "ParticleLink.h"

class ParticleRod : public ParticleLink {
public:
    float max_length;

    bool generate_contact(ParticleContact& contact) const final;
};