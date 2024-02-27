#include "Circle.h"

Circle::Circle(Vec2 pos, Vec2 velocity, float mass, float radius) : Particle(pos, velocity, mass) {
    this->radius = radius;
}