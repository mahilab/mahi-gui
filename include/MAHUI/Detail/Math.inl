#include <iostream>
#include <cassert>
#include <algorithm>
#include <numeric>
#include <iomanip>

namespace carnot {
namespace Math {

template <typename T>
inline std::vector<T> linspace(T a, T b, std::size_t N) {
    std::vector<T> linspace_out(N);
    T delta = (b - a) / (N - 1);
    linspace_out[0] = a;
    for (std::size_t i = 1; i < N - 1; i++)
        linspace_out[i] = linspace_out[i - 1] + delta;
    linspace_out[N - 1] = b;
    return linspace_out;
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

template <typename T>
inline T sum(const std::vector<T>& data) {
    T s = 0;
    for (std::size_t i = 0; i < data.size(); ++i)
        s += data[i];
    return s;
}

template <typename T>
inline T mean(const std::vector<T>& data) {
    T den = static_cast<T>(1) / static_cast<T>(data.size());
    T mean = 0;
    for (std::size_t i = 0; i < data.size(); ++i)
        mean += data[i] * den;
    return mean;
}

template <typename T>
inline T stddev(const std::vector<T>& data, T& meanOut) {
    if (data.size() > 0) {
        meanOut = mean(data);
        std::vector<T> diff(data.size());
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

template <typename T>
inline bool inBounds(const Vector2<T>& position, const Rect<T>& bounds) {
    return position.x >= bounds.left &&
           position.x < bounds.left + bounds.width &&
           position.y >= bounds.top &&
           position.y < bounds.top + bounds.height;
}

template <typename T>
inline bool inBounds(const Vector2<T>& position, const Vector2<T>& size) {
    return position.x >= 0 &&
           position.x < size.x &&
           position.y >= 0 &&
           position.y < size.y;
}

//==============================================================================
// VECTOR ALGEBRA
//==============================================================================

template <typename T>
inline Vector2<T> absVec(const Vector2<T> v) {
    return Vector2<T>(std::abs(v.x), std::abs(v.y));
}

template <typename T>
inline T squaredLength(const Vector2<T> v) {
    return v.x * v.x + v.y * v.y;
}

template <typename T>
inline T magnitude(const Vector2<T>& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

template <typename T>
inline Vector2<T> unit(const Vector2<T>& v) {
    assert(v != Vector2<T>());
    return v / magnitude(v);
}

template <typename T>
inline Vector2<T> normal(const Vector2<T>& v) {
    return Vector2<T>(-v.y, v.x);
}

template <typename T>
inline T dot(const Vector2<T>& lhs, const Vector2<T>& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

template <typename T>
inline T cross(const Vector2<T>& lhs, const Vector2<T>& rhs) {
    return lhs.x * rhs.y - lhs.y * rhs.x;
}


//==============================================================================
// GEOMETRY
//==============================================================================

template <typename T>
inline bool parallel(const Vector2<T>& a1,const Vector2<T>& a2,
                     const Vector2<T>& b1,const Vector2<T>& b2)
{
    return (a2.y - a1.y) * (b2.x - b1.x) == (b2.y - b1.y) * (a2.x - a1.x);
}

template <typename T>
inline bool perpendicular(const Vector2<T>& a1,const Vector2<T>& a2,
                          const Vector2<T>& b1,const Vector2<T>& b2)
{
    return (a2.y - a1.y) * (b2.y - b1.y) == (b1.x - b2.x) * (a2.x - a1.x);
}

template <typename T>
inline bool intersect(const Vector2<T>& a1, const Vector2<T>& a2,
                      const Vector2<T>& b1, const Vector2<T>& b2)
{
    return
    (((b1.x - a1.x) * (a2.y - a1.y) - (b1.y - a1.y) * (a2.x - a1.x)) *
     ((b2.x - a1.x) * (a2.y - a1.y) - (b2.y - a1.y) * (a2.x - a1.x)) < 0)
    &&
    (((a1.x - b1.x) * (b2.y - b1.y) - (a1.y - b1.y) * (b2.x - b1.x)) *
     ((a2.x - b1.x) * (b2.y - b1.y) - (a2.y - b1.y) * (b2.x - b1.x)) < 0);
}


template <typename T>
inline Vector2<T> intersection(const Vector2<T>& a1, const Vector2<T>& a2,
                                 const Vector2<T>& b1, const Vector2<T>& b2)
{
    float v1 = (a1.x * a2.y - a1.y * a2.x);
    float v2 = (b1.x * b2.y - b1.y * b2.x);
    float v3 = ((a1.x - a2.x) * (b1.y - b2.y) - (a1.y - a2.y) * (b1.x - b2.x));

    if (v3 == 0)
        return Vector2<T>(INF,INF);
    else
        return Vector2<T>((v1 * (b1.x - b2.x) - v2 * (a1.x - a2.x)) / v3,
                            (v1 * (b1.y - b2.y) - v2 * (a1.y - a2.y)) / v3);
}

template <typename T>
inline bool insideLine(const Vector2<T>& l1, const Vector2<T>& l2,
                       const Vector2<T>& p) {
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

template <typename T>
inline bool insideTriangle(const Vector2<T>& A, const Vector2<T>& B,
                           const Vector2<T>& C, const Vector2<T>& P)
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

template <typename T>
inline bool insidePolygon(const std::vector<Vector2<T>>& poly,
                          const Vector2<T>& point)
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

template <typename T>
inline T polygonArea(const std::vector<Vector2<T>>& polygon) {
    float area       = 0;
    std::size_t size = polygon.size();
    for (std::size_t i = 1; i < size - 1; ++i)
        area += polygon[i].x * (polygon[i + 1].y - polygon[i - 1].y);
    area += polygon[size - 1].x * (polygon[0].y - polygon[size - 2].y);
    area += polygon[0].x * (polygon[1].y - polygon[size - 1].y);
    return area * static_cast<T>(0.5);
}

template <typename T>
inline bool isConvex(const std::vector<Vector2<T>>& polygon) {
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

template <typename T>
inline T angle(const Vector2<T>& V) {
    return atan2(V.y, V.x);
}

template <typename T>
inline T angle(const Vector2<T>& V1, const Vector2<T> V2) {
    return std::atan2(cross(V1, V2), dot(V1, V2));
}

template <typename T>
inline int winding(const Vector2<T>& a, const Vector2<T> b) {
    return sign(cross(a, b));
}

template <typename T>
inline int winding(const Vector2<T>& a, const Vector2<T>& b, const Vector2<T>& c) {
    return winding((b - a), (c - b));
}

//==============================================================================
// GEOMETRY
//==============================================================================

template <typename T>
inline T perlin(T x, T y) {
    // TODO
}

}  // namespace Math
}  // namespace carnot
