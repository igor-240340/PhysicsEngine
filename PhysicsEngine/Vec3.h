#pragma once

class Vec3
{
public:
    float x;
    float y;
    float z;

public:
    static const Vec3 zero;
    static const Vec3 left;
    static const Vec3 right;
    static const Vec3 up;
    static const Vec3 down;

public:
    static float dot(const Vec3& a, const Vec3& b);

public:
    Vec3();
    Vec3(float x, float y, float z);

    float length() const;
    float length_squared() const;
    Vec3 normalized() const; // Возвращает нормализованную копию вектора.
    
    Vec3 operator+(const Vec3& b);
    Vec3 operator+=(const Vec3& b);
    Vec3 operator-(const Vec3& b);
    Vec3 operator*(float scalar) const;
    Vec3 operator/(float scalar) const;
    Vec3 operator-() const; // Возвращает инвертированную копию вектора.

    friend Vec3 operator*(const float scalar, const Vec3& a);
};