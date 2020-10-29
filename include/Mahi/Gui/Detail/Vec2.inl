namespace mahi {
namespace gui {

//=============================================================================
// VECTOR ALGEBRA
//=============================================================================

inline Vec2 abs_vec(const Vec2& v) {
    return Vec2(std::abs(v.x), std::abs(v.y));
}

inline float sq_len(const Vec2& v) {
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

//=============================================================================
// GEOMETRY
//=============================================================================

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


inline Vec2 intersection(const Vec2& a1, const Vec2& a2, const Vec2& b1, const Vec2& b2)
{
    float v1 = (a1.x * a2.y - a1.y * a2.x);
    float v2 = (b1.x * b2.y - b1.y * b2.x);
    float v3 = ((a1.x - a2.x) * (b1.y - b2.y) - (a1.y - a2.y) * (b1.x - b2.x));

    if (v3 == 0)
        return Vec2((float)util::INF,(float)util::INF);
    else
        return Vec2((v1 * (b1.x - b2.x) - v2 * (a1.x - a2.x)) / v3,
                            (v1 * (b1.y - b2.y) - v2 * (a1.y - a2.y)) / v3);
}

inline bool inside_line(const Vec2& l1, const Vec2& l2,
                       const Vec2& p) {
    float crossproduct = cross(l2 - l1, p - l1);
    if (std::abs(crossproduct) > 0.1)
        return false;
    float dotproduct = dot(l2 - l1, p - l1);
    if (dotproduct < 0)
        return false;
    float squarelength = dot(l2 - l1, l2 - l1);
    if (dotproduct > squarelength)
        return false;
    return true;
}

inline bool inside_triangle(const Vec2& A, const Vec2& B,
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

inline bool inside_polygon(const std::vector<Vec2>& poly,
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

inline float polygon_area(const std::vector<Vec2>& polygon) {
    float area       = 0;
    std::size_t size = polygon.size();
    for (std::size_t i = 1; i < size - 1; ++i)
        area += polygon[i].x * (polygon[i + 1].y - polygon[i - 1].y);
    area += polygon[size - 1].x * (polygon[0].y - polygon[size - 2].y);
    area += polygon[0].x * (polygon[1].y - polygon[size - 1].y);
    return area * 0.5f;
}

inline bool is_convex(const std::vector<Vec2>& polygon) {
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
    return std::atan2(V.y, V.x);
}

inline float angle(const Vec2& V1, const Vec2 V2) {
    return std::atan2(cross(V1, V2), dot(V1, V2));
}

inline int winding(const Vec2& a, const Vec2 b) {
    return util::sign(cross(a, b));
}

inline int winding(const Vec2& a, const Vec2& b, const Vec2& c) {
    return winding((b - a), (c - b));
}

//=============================================================================
// OPERATOR OVERLOADS
//=============================================================================

inline Vec2 operator-(const Vec2 &right)
{
    return Vec2(-right.x, -right.y);
}

inline Vec2 &operator+=(Vec2 &left, const Vec2 &right)
{
    left.x += right.x;
    left.y += right.y;

    return left;
}

inline Vec2 &operator-=(Vec2 &left, const Vec2 &right)
{
    left.x -= right.x;
    left.y -= right.y;

    return left;
}

inline Vec2 operator+(const Vec2 &left, const Vec2 &right)
{
    return Vec2(left.x + right.x, left.y + right.y);
}

inline Vec2 operator-(const Vec2 &left, const Vec2 &right)
{
    return Vec2(left.x - right.x, left.y - right.y);
}

inline Vec2 operator*(const Vec2 &left, float right)
{
    return Vec2(left.x * right, left.y * right);
}

inline Vec2 operator*(float left, const Vec2 &right)
{
    return Vec2(right.x * left, right.y * left);
}

inline Vec2 &operator*=(Vec2 &left, float right)
{
    left.x *= right;
    left.y *= right;

    return left;
}

inline Vec2 operator/(const Vec2 &left, float right)
{
    return Vec2(left.x / right, left.y / right);
}

inline Vec2 &operator/=(Vec2 &left, float right)
{
    left.x /= right;
    left.y /= right;

    return left;
}

inline bool operator==(const Vec2 &left, const Vec2 &right)
{
    return (left.x == right.x) && (left.y == right.y);
}

inline bool operator!=(const Vec2 &left, const Vec2 &right)
{
    return (left.x != right.x) || (left.y != right.y);
}

} // namespace gui
} // namespace mahi