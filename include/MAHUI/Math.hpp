#pragma once

#include <Utility/Types.hpp>
#include <cmath>
#include <vector>
#include <limits>

namespace carnot {

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
template <typename T>
inline std::vector<T> linspace(T a, T b, std::size_t N);

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
template <typename T>
inline T sum(const std::vector<T>& data);

/// Computes the mean of a vector of floats
template <typename T>
inline T mean(const std::vector<T>& data);

/// Computes standard deviation of vector of floats
template <typename T>
inline T stddev(const std::vector<T>& data, T& meanOut);

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

/// Checks if a point is inside a Rect
template <typename T>
inline bool inBounds(const Vector2<T>& position, const Rect<T>& bounds);

/// Checks if a point is inside a given size
template <typename T>
inline bool inBounds(const Vector2<T>& position, const Vector2<T>& size);

//==============================================================================
// VECTOR ALGEBRA
//==============================================================================

/// Returns absolute value of vector
template <typename T>
inline Vector2<T> absVec(const Vector2<T> v);

/// Computes the squared length of a vector
template <typename T>
inline T squaredLength(const Vector2<T> v);

/// Computes the magnitude of a vector
template <typename T>
inline T magnitude(const Vector2<T>& v);

/// Returns the unit vector of a vector
template <typename T>
inline Vector2<T> unit(const Vector2<T>& v);

/// Returns the vector normal to a vector
template <typename T>
inline Vector2<T> normal(const Vector2<T>& v);

/// Computes the dot product of two vectors
template <typename T>
inline T dot(const Vector2<T>& lhs, const Vector2<T>& rhs);

/// Computes the cross product of two vectors
template <typename T>
inline T cross(const Vector2<T>& lhs, const Vector2<T>& rhs);

//==============================================================================
// GEOMETRY
//==============================================================================

/// Returns true if two lines A and B are parallel
template <typename T>
inline bool parallel(const Vector2<T>& a1, const Vector2<T>& a2,
                     const Vector2<T>& b1, const Vector2<T>& b2);

/// Returns true if two lines A and B are perpendicular
template <typename T>
inline bool perpendicular(const Vector2<T>& a1, const Vector2<T>& a2,
                          const Vector2<T>& b1, const Vector2<T>& b2);

/// Returns true if two finite line segments intersect
template <typename T>
inline bool intersect(const Vector2<T>& a1, const Vector2<T>& a2,
                      const Vector2<T>& b1, const Vector2<T>& b2);

/// Finds the intersection point of infinite lines A and B.
/// If A and B are parallel, returns Vector2<T>(INF,INF).
template <typename T>
inline Vector2<T> intersection(const Vector2<T>& a1, const Vector2<T>& a2,
                                 const Vector2<T>& b1, const Vector2<T>& b2);

/// Determines if a point P lies on and within the endpoints of a line L
template <typename T>
inline bool insideLine(const Vector2<T>& l1, const Vector2<T>& l2,
                       const Vector2<T>& p);

/// Determiens if a point P lies inside triangle ABC
template <typename T>
inline bool insideTriangle(const Vector2<T>& a, const Vector2<T>& b,
                           const Vector2<T>& c, const Vector2<T>& p);

/// Returns true if a point is inside a set of polygon points
template <typename T>
inline bool insidePolygon(const std::vector<Vector2<T>>& polygon, const Vector2<T>& p);

/// Computes the area of polygon defined by vector of outer vertices
template <typename T>
inline T polygonArea(const std::vector<Vector2<T>>& polygon);

/// Returns true if a list of simple polygon vertice is convex
template <typename T>
inline bool isConvex(const std::vector<Vector2<T>>& polygon);

/// Computes the angle of a vector (i.e atan2(V.y,V.x))
template <typename T>
inline T angle(const Vector2<T>& V);

/// Computes angle between to vectors V1 and V2
template <typename T>
inline T angle(const Vector2<T>& V1, const Vector2<T> V2);

/// Returns winding of two vectors
template <typename T>
inline int winding(const Vector2<T>& a, const Vector2<T> b);

/// Returns 1 if three conesecutive points are in clockwise order,
/// -1 if counter-clockwise, and 0 if colinear
template <typename T>
inline int winding(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c);

//==============================================================================
// NOISE
//==============================================================================

/// Genrates 2D perlin noise
template <typename T>
inline T perlin(T x, T y);

}  // namespace Math
}  // namespace carnot

#include <Utility/Detail/Math.inl>
