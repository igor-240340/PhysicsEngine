#pragma once

#include "PhysicsEngine/Particle.h"

class Circle : public Particle
{
public:
    float radius;

public:
    Circle(Vec3 pos, Vec3 velocity, float mass, float radius);
};