#pragma once

namespace mahi::gui
{

class Vec2
{
public:
    Vec2();
    Vec2(float X, float Y);

public:
    float x; 
    float y; 
};

Vec2 operator-(const Vec2 &right);

Vec2 &operator+=(Vec2 &left, const Vec2 &right);

Vec2 &operator-=(Vec2 &left, const Vec2 &right);

Vec2 operator+(const Vec2 &left, const Vec2 &right);

Vec2 operator-(const Vec2 &left, const Vec2 &right);

Vec2 operator*(const Vec2 &left, float right);

Vec2 operator*(float left, const Vec2 &right);

Vec2 &operator*=(Vec2 &left, float right);

Vec2 operator/(const Vec2 &left, float right);

Vec2 &operator/=(Vec2 &left, float right);

bool operator==(const Vec2 &left, const Vec2 &right);

bool operator!=(const Vec2 &left, const Vec2 &right);

#include <mahi/Detail/Vec2.inl>

} // namespace mahi::gui