#include "Circle.h"

Circle::Circle(Vec3 pos, Vec3 velocity, float mass, float radius) : Particle(pos, velocity, mass) {
    this->radius = radius;
}