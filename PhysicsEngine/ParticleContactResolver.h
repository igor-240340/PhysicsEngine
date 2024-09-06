#pragma once

#include <list>

#include "ParticleContact.h"

class ParticleContactResolver {
public:
    void resolve_contacts(std::list<ParticleContact*>& contacts);
};
