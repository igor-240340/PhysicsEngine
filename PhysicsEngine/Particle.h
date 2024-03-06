#pragma once

#include "Vec2.h"

class Particle
{
public:
    Vec2 pos;
    Vec2 velocity;
    Vec2 netForce;

    float mass;
    float invMass;

public:
    Particle(Vec2 pos, Vec2 velocity, float mass);

    virtual void ApplyForce(Vec2 force);
};