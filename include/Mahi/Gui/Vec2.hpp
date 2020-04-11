// MIT License
//
// Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#pragma once

//#include <Mahi/Gui/Vec2.hpp>
#include <Mahi/Util/Math/Constants.hpp>
#include <Mahi/Util/Math/Functions.hpp>
#include <cmath>
#include <limits>
#include <vector>

namespace mahi {
namespace gui {

struct Vec2 {
    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}
    float  operator[](std::size_t i) const { return (&x)[i]; }
    float &operator[](std::size_t i) { return (&x)[i]; }
    float  x;
    float  y;
};

//=============================================================================
// VECTOR ALGEBRA
//=============================================================================

/// Returns absolute value of vector
inline Vec2 abs_vec(const Vec2 &v);
/// Computes the squared length of a vector
inline float sq_len(const Vec2 &v);
/// Computes the magnitude of a vector
inline float magnitude(const Vec2 &v);
/// Returns the unit vector of a vector
inline Vec2 unit(const Vec2 &v);
/// Returns the vector normal to a vector
inline Vec2 normal(const Vec2 &v);
/// Computes the dot product of two vectors
inline float dot(const Vec2 &lhs, const Vec2 &rhs);
/// Computes the cross product of two vectors
inline float cross(const Vec2 &lhs, const Vec2 &rhs);

//=============================================================================
// GEOMETRY
//=============================================================================

/// Returns true if two lines A and B are parallel
inline bool parallel(const Vec2 &a1, const Vec2 &a2, const Vec2 &b1, const Vec2 &b2);
/// Returns true if two lines A and B are perpendicular
inline bool perpendicular(const Vec2 &a1, const Vec2 &a2, const Vec2 &b1, const Vec2 &b2);
/// Returns true if two finite line segments intersect
inline bool intersect(const Vec2 &a1, const Vec2 &a2, const Vec2 &b1, const Vec2 &b2);
/// Finds the intersection point of infinite lines A and B.
/// If A and B are parallel, returns Vec2(INF,INF).
inline Vec2 intersection(const Vec2 &a1, const Vec2 &a2, const Vec2 &b1, const Vec2 &b2);
/// Determines if a point P lies on and within the endpoints of a line L
inline bool inside_line(const Vec2 &l1, const Vec2 &l2, const Vec2 &p);
/// Determiens if a point P lies inside triangle ABC
inline bool inside_triangle(const Vec2 &a, const Vec2 &b, const Vec2 &c, const Vec2 &p);
/// Returns true if a point is inside a set of polygon points
inline bool inside_polygon(const std::vector<Vec2> &polygon, const Vec2 &p);
/// Computes the area of polygon defined by vector of outer vertices
inline float polygon_area(const std::vector<Vec2> &polygon);
/// Returns true if a list of simple polygon vertice is convex
inline bool is_convex(const std::vector<Vec2> &polygon);
/// Computes the angle of a vector (i.e atan2(V.y,V.x))
inline float angle(const Vec2 &V);
/// Computes angle between to vectors V1 and V2
inline float angle(const Vec2 &V1, const Vec2 V2);
/// Returns winding of two vectors
inline int winding(const Vec2 &a, const Vec2 b);
/// Returns 1 if three consecutive points are in clockwise order,
/// -1 if counter-clockwise, and 0 if colinear
inline int winding(const Vec2 &a, const Vec2 &b, const Vec2 &c);

//=============================================================================
// OPERATOR OVERLOADS
//=============================================================================

Vec2  operator-(const Vec2 &right);
Vec2 &operator+=(Vec2 &left, const Vec2 &right);
Vec2 &operator-=(Vec2 &left, const Vec2 &right);
Vec2  operator+(const Vec2 &left, const Vec2 &right);
Vec2  operator-(const Vec2 &left, const Vec2 &right);
Vec2  operator*(const Vec2 &left, float right);
Vec2  operator*(float left, const Vec2 &right);
Vec2 &operator*=(Vec2 &left, float right);
Vec2  operator/(const Vec2 &left, float right);
Vec2 &operator/=(Vec2 &left, float right);
bool  operator==(const Vec2 &left, const Vec2 &right);
bool  operator!=(const Vec2 &left, const Vec2 &right);

}  // namespace gui
}  // namespace mahi

#include <Mahi/Gui/Detail/Vec2.inl>