#include <cmath>

#include "Vec2.h"

const Vec2 Vec2::Zero = Vec2(0.0f, 0.0f);
const Vec2 Vec2::Down = Vec2(0.0f, -1.0f);

Vec2::Vec2() {
    x = 0.0f;
    y = 0.0f;
}

Vec2::Vec2(float x, float y) {
    this->x = x;
    this->y = y;
}

float Vec2::Length() const {
    return std::sqrt(x * x + y * y);
}

Vec2 Vec2::GetNormal() const {
    return *this / Length();
}

float Vec2::Dot(const Vec2& a, const Vec2& b) {
    return a.x * b.x + a.y * b.y;
}

Vec2 Vec2::operator+(const Vec2& b) {
    return Vec2(x + b.x, y + b.y);
}

Vec2 Vec2::operator+=(const Vec2& b) {
    x += b.x;
    y += b.y;

    return *this;
}

Vec2 Vec2::operator-(const Vec2& b) {
    return Vec2(x - b.x, y - b.y);
}

Vec2 Vec2::operator*(float scalar) const {
    return Vec2(x * scalar, y * scalar);
}

Vec2 Vec2::operator/(float scalar) const {
    float scalarInv = 1.0f / scalar;
    return Vec2(x * scalarInv, y * scalarInv);
}

Vec2 Vec2::operator-() const {
    return Vec2(-x, -y);
}