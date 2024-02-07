#pragma once

class Vec2
{
public:
    float x;
    float y;

public:
    static const Vec2 Zero;
    static const Vec2 Left;
    static const Vec2 Right;
    static const Vec2 Up;
    static const Vec2 Down;

public:
    static float Dot(const Vec2& a, const Vec2& b);

public:
    Vec2();
    Vec2(float x, float y);

    float Length() const;
    Vec2 Normalized() const; // Возвращает нормализованную копию вектора.
    
    Vec2 operator+(const Vec2& b);
    Vec2 operator+=(const Vec2& b);
    Vec2 operator-(const Vec2& b);
    Vec2 operator*(float scalar) const;
    Vec2 operator/(float scalar) const;
    Vec2 operator-() const; // Возвращает инвертированную копию вектора.

    friend Vec2 operator*(const float scalar, const Vec2& a);
};