#include "ParticleContactResolver.h"

void ParticleContactResolver::resolve_contacts(std::list<ParticleContact*>& contacts) {
    for (ParticleContact* contact : contacts) {
        contact->resolve();
        delete contact;
    }
}
