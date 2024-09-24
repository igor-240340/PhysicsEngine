#pragma once

#include "ParticleLink.h"

class ParticleRod : public ParticleLink {
public:
    float length;

    bool generate_contact(ParticleContact& contact) const final;
};