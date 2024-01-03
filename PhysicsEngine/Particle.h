#pragma once

#include "Vec2.h"

class Particle
{
public:
    Vec2 pos;
    Vec2 netForce;
    Vec2 velocity;

    float mass;
    float invMass;

public:
    Particle(Vec2 pos, Vec2 velocity, float mass);

    void ApplyForce(Vec2 force);
};