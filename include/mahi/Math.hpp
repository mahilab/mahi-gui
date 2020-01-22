#pragma once

#include <cmath>
#include <vector>
#include <limits>
#include <mahi/Vec2.hpp>

namespace mahi::gui {

namespace Math {

//==============================================================================
// USINGS
//==============================================================================

using std::abs;
using std::fmod;
using std::remainder;

using std::exp;
using std::log;
using std::log10;
using std::log2;

using std::pow;
using std::sqrt;
using std::cbrt;
using std::hypot;

using std::sin;
using std::cos;
using std::tan;
using std::asin;
using std::acos;
using std::atan;
using std::atan2;

using std::sinh;
using std::cosh;
using std::tanh;
using std::asinh;
using std::acosh;
using std::atanh;

using std::ceil;
using std::floor;
using std::trunc;
using std::round;

//==============================================================================
// CONSTANTS
//==============================================================================

static const float PI      = 3.141592653589793238463f;
static const float HALFPI  = 1.570796326794896619231f;
static const float TWOPI   = 6.283185307179586476925f;
static const float DEG2RAD = 0.017453292519943295769f;
static const float RAD2DEG = 57.29577951308232087680f;
static const float SQRT2   = 1.414213562373095048802f;
static const float SQRT3   = 1.732050807568877293528f;
static const float INF     = std::numeric_limits<float>::infinity();
static const float EPS     = std::numeric_limits<float>::epsilon();

//==============================================================================
// GENERAL
//==============================================================================

/// Creates an evenly space array of N values between a and b
template <typename T, typename TArray>
inline void linspace(T a, T b, TArray& array);

/// Clamps a float between min and max
template <typename T>
inline T clamp(T value, T min, T max);

/// Clamps a float between 0 and 1
template <typename T>
inline T clamp01(T value);

/// Wraps an angle in radians to within [-PI, PI)
template <typename T>
inline T wrapToPi(T angle);

/// Wraps an angle in radians to within [0, 2*PI)
template <typename T>
inline T wrapTo2Pi(T angle);

/// Determines if two floats a and b are approximately equal
template <typename T>
inline bool approximately(T a, T b, T delta = EPS);

/// Determins sign of a number (-1 negative, +1 positive, or 0)
template <typename T>
inline int sign(T value);

/// Returns true if an integer is even
inline bool isEven(int value);

/// Returns true if an integer is odd
inline bool isOdd(int value);

/// Computes the sum of a vector of floats
template <typename T, typename TArray>
inline T sum(const TArray& data);

/// Computes the mean of a vector of floats
template <typename T, typename TArray>
inline T mean(const TArray& data);

/// Computes standard deviation of vector of floats
template <typename T, typename TArray>
inline T stddev(const TArray& data, T& meanOut);

/// Interpolate values
template <typename T>
inline T interp(T x, T x0, T x1, T y0, T y1);

/// Determine order of magnitude of number
template <typename T>
inline int orderOfMagnitude(T value);

/// Precison
inline std::size_t precision(int order);

/// Rounds a float up to the nearest interval
template <typename T>
inline T roundUpToNearest(T value, T interval);

/// Rounds a float down to the nearest interval
template <typename T>
inline T roundDownToNearest(T value, T interval);

/// Rounds a float up or down to the nearest interval
template <typename T>
inline T roundToNearest(T value, T interval);

//==============================================================================
// VECTOR ALGEBRA
//==============================================================================

/// Returns absolute value of vector
inline Vec2 absVec(const Vec2& v);

/// Computes the squared length of a vector
inline float squaredLength(const Vec2& v);

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

//==============================================================================
// GEOMETRY
//==============================================================================

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

//==============================================================================
// NOISE
//==============================================================================

}  // namespace Math
}  // namespace mahi::gui

#include <mahi/Detail/Math.inl>
