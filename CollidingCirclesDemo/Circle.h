#pragma once

#include "PhysicsEngine/Particle.h"

class Circle : public Particle
{
public:
    float radius;

public:
    Circle(Vec2 pos, Vec2 velocity, float mass, float radius);
};