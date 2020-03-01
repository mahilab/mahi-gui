#include <iostream>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <iomanip>

namespace mahi::gui {
namespace Math {

template <typename T, typename TArray>
inline void linspace(T a, T b, TArray& array) {
    if (array.size() == 0)
        return;
    std::size_t N = array.size();
    T delta = (b - a) / (N - 1);
    array[0] = a;
    for (std::size_t i = 1; i < N - 1; i++)
        array[i] = array[i - 1] + delta;
    array[N - 1] = b;
    return array;
}

template <typename T>
inline T clamp(T value, T min, T max) {
    return value <= min ? min : value >= max ? max : value;
}

template <typename T>
inline T clamp01(T value) {
    return value <= 0 ? 0 : value >= 1 ? 1 : value;
}

template <typename T>
inline T wrapToPi(T angle) {
    angle = std::fmod(angle + PI, TWOPI);
    if (angle < 0)
        angle += TWOPI;
    return angle - PI;
}


template <typename T>
inline T wrapTo2Pi(T angle) {
    angle = std::fmod(angle, TWOPI);
    if (angle < 0)
        angle += TWOPI;
    return angle;
}

template <typename T>
inline bool approximately(T a, T b, T delta) {
    return std::abs(a-b) < delta;
}

template <typename T>
inline int sign(T value) {
    return (0 < value) - (value < 0);
}

inline bool isEven(int value) {
    return !(value % 2);
}

inline bool isOdd(int value) {
    return (value % 2);
}

template <typename T, typename TArray>
inline T sum(const TArray& data) {
    T s = 0;
    for (std::size_t i = 0; i < data.size(); ++i)
        s += data[i];
    return s;
}

template <typename T, typename TArray>
inline T mean(const TArray& data) {
    T den = static_cast<T>(1) / static_cast<T>(data.size());
    T mean = 0;
    for (std::size_t i = 0; i < data.size(); ++i)
        mean += data[i] * den;
    return mean;
}

template <typename T, typename TArray>
inline T stddev(const TArray& data, T& meanOut) {
    if (data.size() > 0) {
        meanOut = mean(data);
        TArray diff(data.size());
        std::transform(data.begin(), data.end(), diff.begin(), [meanOut](T x) { return x - meanOut; });
        T sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0);
        return std::sqrt(sq_sum / data.size());
    }
    else {
        return 0;
    }
}

template <typename T>
inline T interp(T x, T x0, T x1, T y0, T y1) {
    return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}

template <typename T>
inline int orderOfMagnitude(T value) {
    if (value == 0)
        return 0;
    else
        return static_cast<int>(floor(log10(std::abs(value))));
}

inline std::size_t precision(int order) {
    std::size_t prec;
    if (order >= 1)
        prec = 0;
    else if (order == 0)
        prec = 1;
    else
        prec = -order + 1;
    return prec;
}

template <typename T>
inline T roundUpToNearest(T value, T interval)
{
    if (interval == 0)
        return value;
    T rem = fmod(abs(value), interval);
    if (rem == 0)
        return value;
    if (value < 0)
        return -(abs(value) - rem);
    else
        return value + interval - rem;
}

template <typename T>
inline T roundDownToNearest(T value, T interval) {
    if (interval == 0)
        return value;
    T rem = fmod(abs(value), interval);
    if (rem == 0)
        return value;
    if (value < 0)
        return -(abs(value) - rem);
    else
        return value - rem;
}

template <typename T>
inline T roundToNearest(T value, T interval) {
    if (value >= 0) {
        T rem = fmod(value, interval);
        value = rem > interval * static_cast<T>(0.5) ? value + interval - rem : value - rem;
    }
    else {
        value = -value;
        T rem = fmod(value, interval);
        value = rem > interval * static_cast<T>(0.5) ? value + interval - rem : value - rem;
        value = -value;

    }
    return value;
}

//==============================================================================
// VECTOR ALGEBRA
//==============================================================================

inline Vec2 absVec(const Vec2& v) {
    return Vec2(std::abs(v.x), std::abs(v.y));
}

inline float squaredLength(const Vec2& v) {
    return v.x * v.x + v.y * v.y;
}

inline float magnitude(const Vec2& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

inline Vec2 unit(const Vec2& v) {
    assert(v != Vec2());
    return v * 1.0f / magnitude(v);
}

inline Vec2 normal(const Vec2& v) {
    return Vec2(-v.y, v.x);
}

inline float dot(const Vec2& lhs, const Vec2& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

inline float cross(const Vec2& lhs, const Vec2& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}


//==============================================================================
// GEOMETRY
//==============================================================================

inline bool parallel(const Vec2& a1,const Vec2& a2,
                     const Vec2& b1,const Vec2& b2)
{
    return (a2.y - a1.y) * (b2.x - b1.x) == (b2.y - b1.y) * (a2.x - a1.x);
}

inline bool perpendicular(const Vec2& a1,const Vec2& a2,
                          const Vec2& b1,const Vec2& b2)
{
    return (a2.y - a1.y) * (b2.y - b1.y) == (b1.x - b2.x) * (a2.x - a1.x);
}

inline bool intersect(const Vec2& a1, const Vec2& a2,
                      const Vec2& b1, const Vec2& b2)
{
    return
    (((b1.x - a1.x) * (a2.y - a1.y) - (b1.y - a1.y) * (a2.x - a1.x)) *
     ((b2.x - a1.x) * (a2.y - a1.y) - (b2.y - a1.y) * (a2.x - a1.x)) < 0)
    &&
    (((a1.x - b1.x) * (b2.y - b1.y) - (a1.y - b1.y) * (b2.x - b1.x)) *
     ((a2.x - b1.x) * (b2.y - b1.y) - (a2.y - b1.y) * (b2.x - b1.x)) < 0);
}


inline Vec2 intersection(const Vec2& a1, const Vec2& a2,
                                 const Vec2& b1, const Vec2& b2)
{
    float v1 = (a1.x * a2.y - a1.y * a2.x);
    float v2 = (b1.x * b2.y - b1.y * b2.x);
    float v3 = ((a1.x - a2.x) * (b1.y - b2.y) - (a1.y - a2.y) * (b1.x - b2.x));

    if (v3 == 0)
        return Vec2(INF,INF);
    else
        return Vec2((v1 * (b1.x - b2.x) - v2 * (a1.x - a2.x)) / v3,
                            (v1 * (b1.y - b2.y) - v2 * (a1.y - a2.y)) / v3);
}

inline bool insideLine(const Vec2& l1, const Vec2& l2,
                       const Vec2& p) {
    float crossproduct = cross(l2 - l1, p - l1);
    if (abs(crossproduct) > 0.1)
        return false;
    float dotproduct = dot(l2 - l1, p - l1);
    if (dotproduct < 0)
        return false;
    float squarelength = dot(l2 - l1, l2 - l1);
    if (dotproduct > squarelength)
        return false;
    return true;
}

inline bool insideTriangle(const Vec2& A, const Vec2& B,
                           const Vec2& C, const Vec2& P)
{
    float s = A.y * C.x - A.x * C.y + (C.y - A.y) * P.x + (A.x - C.x) * P.y;
    float t = A.x * B.y - A.y * B.x + (A.y - B.y) * P.x + (B.x - A.x) * P.y;

    if ((s < 0) != (t < 0))
        return false;

    float area = -B.y * C.x + A.y * (C.x - B.x) + A.x * (B.y - C.y) + B.x * C.y;
    if (area < 0.0) {
        s    = -s;
        t    = -t;
        area = -area;
    }
    return s > 0 && t > 0 && (s + t) <= area;
}

inline bool insidePolygon(const std::vector<Vec2>& poly,
                          const Vec2& point)
{
    std::size_t i, j;
    std::size_t N = poly.size();
    bool c = false;
    for (i = 0, j = N-1; i < N; j = i++) {
    if ( ((poly[i].y>point.y) != (poly[j].y>point.y)) &&
        (point.x < (poly[j].x-poly[i].x) * (point.y-poly[i].y) / (poly[j].y-poly[i].y) + poly[i].x) )
        c = !c;
    }
    return c;
}

inline float polygonArea(const std::vector<Vec2>& polygon) {
    float area       = 0;
    std::size_t size = polygon.size();
    for (std::size_t i = 1; i < size - 1; ++i)
        area += polygon[i].x * (polygon[i + 1].y - polygon[i - 1].y);
    area += polygon[size - 1].x * (polygon[0].y - polygon[size - 2].y);
    area += polygon[0].x * (polygon[1].y - polygon[size - 1].y);
    return area * 0.5f;
}

inline bool isConvex(const std::vector<Vec2>& polygon) {
    bool negative = false;
    bool positive = false;
    for (std::size_t a = 0; a < polygon.size(); ++a) {
        auto b = (a+1)%polygon.size();
        auto c = (b+1)%polygon.size();
        auto prod = cross(polygon[b]-polygon[a],polygon[c]-polygon[b]);
        if (prod < 0)
            negative = true;
        else if (prod > 0)
            positive = true;
        if (negative && positive)
            return false;        
    }
    return true;
}

inline float angle(const Vec2& V) {
    return atan2(V.y, V.x);
}

inline float angle(const Vec2& V1, const Vec2 V2) {
    return std::atan2(cross(V1, V2), dot(V1, V2));
}

inline int winding(const Vec2& a, const Vec2 b) {
    return sign(cross(a, b));
}

inline int winding(const Vec2& a, const Vec2& b, const Vec2& c) {
    return winding((b - a), (c - b));
}

}  // namespace Math
}  // namespace mahi::gui
