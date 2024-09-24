#pragma once

#include "Vec3.h"

class Particle
{
public:
    Vec3 pos;
    Vec3 velocity;
    Vec3 net_force;

    float mass;
    float inv_mass;

public:
    Particle();
    Particle(Vec3 pos, Vec3 velocity, float mass);

    virtual void ApplyForce(Vec3 force);
};