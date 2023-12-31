#pragma once

class Vec2
{
public:
    float x;
    float y;

public:
    static const Vec2 Zero;
    static const Vec2 Down;

public:
    static float Dot(const Vec2& a, const Vec2& b);

public:
    Vec2();
    Vec2(float x, float y);

    float Length() const;
    Vec2 GetNormal() const; // ���������� ��������������� ����� �������.

    Vec2 operator+(const Vec2& b);
    Vec2 operator+=(const Vec2& b);
    Vec2 operator-(const Vec2& b);
    Vec2 operator*(float scalar) const;
    Vec2 operator/(float scalar) const;
    Vec2 operator-() const; // ���������� ��������������� ����� �������.
};