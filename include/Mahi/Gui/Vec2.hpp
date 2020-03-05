#pragma once

#include <cmath>
#include <vector>
#include <limits>
#include <Mahi/Gui/Vec2.hpp>
#include <Mahi/Util/Math/Constants.hpp>
#include <Mahi/Util/Math/Functions.hpp>

namespace mahi {
namespace gui {

class Vec2
{
public:
    Vec2();
    Vec2(float X, float Y);

public:
    float x; 
    float y; 
};

//=============================================================================
// VECTOR ALGEBRA
//=============================================================================

/// Returns absolute value of vector
inline Vec2 abs_vec(const Vec2& v);

/// Computes the squared length of a vector
inline float sq_len(const Vec2& v);

/// Computes the magnitude of a vector
inline float magnitude(const Vec2& v);

/// Returns the unit vector of a vector
inline Vec2 unit(const Vec2& v);

/// Returns the vector normal to a vector
inline Vec2 normal(const Vec2& v);

/// Computes the dot product of two vectors
inline float dot(const Vec2& lhs, const Vec2& rhs);

/// Computes the cross product of two vectors
inline float cross(const Vec2& lhs, const Vec2& rhs);

//=============================================================================
// GEOMETRY
//=============================================================================

/// Returns true if two lines A and B are parallel
inline bool parallel(const Vec2 a1, const Vec2 a2,
                     const Vec2 b1, const Vec2 b2);

/// Returns true if two lines A and B are perpendicular
inline bool perpendicular(const Vec2 a1, const Vec2 a2,
                          const Vec2 b1, const Vec2 b2);

/// Returns true if two finite line segments intersect
inline bool intersect(const Vec2 a1, const Vec2 a2,
                      const Vec2 b1, const Vec2 b2);

/// Finds the intersection point of infinite lines A and B.
/// If A and B are parallel, returns Vec2(INF,INF).
inline Vec2 intersection(const Vec2 a1, const Vec2 a2,
                        const Vec2 b1, const Vec2 b2);

/// Determines if a point P lies on and within the endpoints of a line L
inline bool insideLine(const Vec2 l1, const Vec2 l2,
                       const Vec2 p);

/// Determiens if a point P lies inside triangle ABC
inline bool insideTriangle(const Vec2 a, const Vec2 b,
                           const Vec2 c, const Vec2 p);

/// Returns true if a point is inside a set of polygon points
inline bool insidePolygon(const std::vector<Vec2>& polygon, const Vec2& p);

/// Computes the area of polygon defined by vector of outer vertices
inline float polygonArea(const std::vector<Vec2>& polygon);

/// Returns true if a list of simple polygon vertice is convex
inline bool isConvex(const std::vector<Vec2>& polygon);

/// Computes the angle of a vector (i.e atan2(V.y,V.x))
inline float angle(const Vec2& V);

/// Computes angle between to vectors V1 and V2
inline float angle(const Vec2& V1, const Vec2 V2);

/// Returns winding of two vectors
inline int winding(const Vec2& a, const Vec2 b);

/// Returns 1 if three conesecutive points are in clockwise order,
/// -1 if counter-clockwise, and 0 if colinear
inline int winding(const Vec2& a, const Vec2& b, const Vec2& c);

//=============================================================================
// OPERATOR OVERLOADS
//=============================================================================

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

} // namespace mahi
} // namespace mahi

#include <Mahi/Gui/Detail/Vec2.inl>