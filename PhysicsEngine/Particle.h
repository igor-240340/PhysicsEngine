#pragma once

#include "Vec2.h"

class Particle
{
public:
    Vec3 pos;
    Vec3 velocity;
    Vec3 netForce;

    float mass;
    float invMass;

public:
    Particle(Vec3 pos, Vec3 velocity, float mass);

    virtual void ApplyForce(Vec3 force);
};