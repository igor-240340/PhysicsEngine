#pragma once

#include "ParticleContact.h"

class ParticleContactGenerator
{
public:
    virtual bool generate_contact(ParticleContact& contact) const = 0;
};
