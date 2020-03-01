inline Vec2::Vec2() : x(0),
                      y(0)
{
}

inline Vec2::Vec2(float X, float Y) : x(X),
                                      y(Y)
{
}

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