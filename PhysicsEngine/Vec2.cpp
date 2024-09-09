#include <cmath>

#include "Vec2.h"

const Vec3 Vec3::zero = Vec3(0.0f, 0.0f, 0.0f);
const Vec3 Vec3::left = Vec3(-1.0f, 0.0f, 0.0f);
const Vec3 Vec3::right = Vec3(1.0f, 0.0f, 0.0f);
const Vec3 Vec3::up = Vec3(0.0f, 1.0f, 0.0f);
const Vec3 Vec3::down = Vec3(0.0f, -1.0f, 0.0f);

Vec3::Vec3() {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
}

Vec3::Vec3(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

float Vec3::length() const {
    return std::sqrt(x * x + y * y);
}

float Vec3::length_squared() const {
    return x * x + y * y;
}

Vec3 Vec3::normalized() const {
    // Исключаем появление NaN в компонентах вектора.
    if (x * x + y * y > 0)
        return *this / length();
    else
        return Vec3::zero;
}

float Vec3::dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 Vec3::operator+(const Vec3& b) {
    return Vec3(x + b.x, y + b.y, z + b.z);
}

Vec3 Vec3::operator+=(const Vec3& b) {
    x += b.x;
    y += b.y;
    z += b.z;

    return *this;
}

Vec3 Vec3::operator-(const Vec3& b) {
    return Vec3(x - b.x, y - b.y, z - b.z);
}

Vec3 Vec3::operator*(float scalar) const {
    return Vec3(x * scalar, y * scalar, z * scalar);
}

Vec3 Vec3::operator/(float scalar) const {
    float scalar_inv = 1.0f / scalar;
    return Vec3(x * scalar_inv, y * scalar_inv, z * scalar_inv);
}

Vec3 Vec3::operator-() const {
    return Vec3(-x, -y, -z);
}

Vec3 operator*(const float scalar, const Vec3& a) {
    return a * scalar;
}