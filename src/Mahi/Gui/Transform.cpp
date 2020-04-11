#include <Mahi/Gui/Transform.hpp>
#include <cmath>

namespace mahi {
namespace gui {

const Transform Transform::Identity;

Transform::Transform() {
    // Identity matrix
    m_matrix[0]  = 1.f;
    m_matrix[4]  = 0.f;
    m_matrix[8]  = 0.f;
    m_matrix[12] = 0.f;
    m_matrix[1]  = 0.f;
    m_matrix[5]  = 1.f;
    m_matrix[9]  = 0.f;
    m_matrix[13] = 0.f;
    m_matrix[2]  = 0.f;
    m_matrix[6]  = 0.f;
    m_matrix[10] = 1.f;
    m_matrix[14] = 0.f;
    m_matrix[3]  = 0.f;
    m_matrix[7]  = 0.f;
    m_matrix[11] = 0.f;
    m_matrix[15] = 1.f;
}

Transform::Transform(float a00, float a01, float a02, float a10, float a11, float a12, float a20,
                     float a21, float a22) {
    m_matrix[0]  = a00;
    m_matrix[4]  = a01;
    m_matrix[8]  = 0.f;
    m_matrix[12] = a02;
    m_matrix[1]  = a10;
    m_matrix[5]  = a11;
    m_matrix[9]  = 0.f;
    m_matrix[13] = a12;
    m_matrix[2]  = 0.f;
    m_matrix[6]  = 0.f;
    m_matrix[10] = 1.f;
    m_matrix[14] = 0.f;
    m_matrix[3]  = a20;
    m_matrix[7]  = a21;
    m_matrix[11] = 0.f;
    m_matrix[15] = a22;
}

const float* Transform::matrix() const { return m_matrix; }

Transform Transform::inverse() const {
    // Compute the determinant
    float det = m_matrix[0] * (m_matrix[15] * m_matrix[5] - m_matrix[7] * m_matrix[13]) -
                m_matrix[1] * (m_matrix[15] * m_matrix[4] - m_matrix[7] * m_matrix[12]) +
                m_matrix[3] * (m_matrix[13] * m_matrix[4] - m_matrix[5] * m_matrix[12]);

    // Compute the inverse if the determinant is not zero
    // (don't use an epsilon because the determinant may *really* be tiny)
    if (det != 0.f) {
        return Transform((m_matrix[15] * m_matrix[5] - m_matrix[7] * m_matrix[13]) / det,
                         -(m_matrix[15] * m_matrix[4] - m_matrix[7] * m_matrix[12]) / det,
                         (m_matrix[13] * m_matrix[4] - m_matrix[5] * m_matrix[12]) / det,
                         -(m_matrix[15] * m_matrix[1] - m_matrix[3] * m_matrix[13]) / det,
                         (m_matrix[15] * m_matrix[0] - m_matrix[3] * m_matrix[12]) / det,
                         -(m_matrix[13] * m_matrix[0] - m_matrix[1] * m_matrix[12]) / det,
                         (m_matrix[7] * m_matrix[1] - m_matrix[3] * m_matrix[5]) / det,
                         -(m_matrix[7] * m_matrix[0] - m_matrix[3] * m_matrix[4]) / det,
                         (m_matrix[5] * m_matrix[0] - m_matrix[1] * m_matrix[4]) / det);
    } else {
        return Identity;
    }
}

Vec2 Transform::transform(float x, float y) const {
    return Vec2(m_matrix[0] * x + m_matrix[4] * y + m_matrix[12],
                m_matrix[1] * x + m_matrix[5] * y + m_matrix[13]);
}

Vec2 Transform::transform(const Vec2& point) const { return transform(point.x, point.y); }

Rect Transform::transform(const Rect& rectangle) const {
    // Transform the 4 corners of the rectangle
    const Vec2 points[] = {
        transform(rectangle.left, rectangle.top),
        transform(rectangle.left, rectangle.top + rectangle.height),
        transform(rectangle.left + rectangle.width, rectangle.top),
        transform(rectangle.left + rectangle.width, rectangle.top + rectangle.height)};

    // Compute the bounding rectangle of the transformed points
    float left   = points[0].x;
    float top    = points[0].y;
    float right  = points[0].x;
    float bottom = points[0].y;
    for (int i = 1; i < 4; ++i) {
        if (points[i].x < left)
            left = points[i].x;
        else if (points[i].x > right)
            right = points[i].x;
        if (points[i].y < top)
            top = points[i].y;
        else if (points[i].y > bottom)
            bottom = points[i].y;
    }

    return Rect(left, top, right - left, bottom - top);
}

Transform& Transform::combine(const Transform& transform) {
    const float* a = m_matrix;
    const float* b = transform.m_matrix;

    *this = Transform(
        a[0] * b[0] + a[4] * b[1] + a[12] * b[3], a[0] * b[4] + a[4] * b[5] + a[12] * b[7],
        a[0] * b[12] + a[4] * b[13] + a[12] * b[15], a[1] * b[0] + a[5] * b[1] + a[13] * b[3],
        a[1] * b[4] + a[5] * b[5] + a[13] * b[7], a[1] * b[12] + a[5] * b[13] + a[13] * b[15],
        a[3] * b[0] + a[7] * b[1] + a[15] * b[3], a[3] * b[4] + a[7] * b[5] + a[15] * b[7],
        a[3] * b[12] + a[7] * b[13] + a[15] * b[15]);

    return *this;
}

Transform& Transform::translate(float x, float y) {
    Transform translation(1, 0, x, 0, 1, y, 0, 0, 1);

    return combine(translation);
}

Transform& Transform::translate(const Vec2& offset) { return translate(offset.x, offset.y); }

Transform& Transform::rotate(float angle) {
    float rad = angle * 3.141592654f / 180.f;
    float cos = std::cos(rad);
    float sin = std::sin(rad);

    Transform rotation(cos, -sin, 0, sin, cos, 0, 0, 0, 1);

    return combine(rotation);
}

Transform& Transform::rotate(float angle, float centerX, float centerY) {
    float rad = angle * 3.141592654f / 180.f;
    float cos = std::cos(rad);
    float sin = std::sin(rad);

    Transform rotation(cos, -sin, centerX * (1 - cos) + centerY * sin, sin, cos,
                       centerY * (1 - cos) - centerX * sin, 0, 0, 1);

    return combine(rotation);
}

Transform& Transform::rotate(float angle, const Vec2& center) {
    return rotate(angle, center.x, center.y);
}

Transform& Transform::scale(float scaleX, float scaleY) {
    Transform scaling(scaleX, 0, 0, 0, scaleY, 0, 0, 0, 1);

    return combine(scaling);
}

Transform& Transform::scale(float scaleX, float scaleY, float centerX, float centerY) {
    Transform scaling(scaleX, 0, centerX * (1 - scaleX), 0, scaleY, centerY * (1 - scaleY), 0, 0,
                      1);

    return combine(scaling);
}

Transform& Transform::scale(const Vec2& factors) { return scale(factors.x, factors.y); }

Transform& Transform::scale(const Vec2& factors, const Vec2& center) {
    return scale(factors.x, factors.y, center.x, center.y);
}

Transform operator*(const Transform& left, const Transform& right) {
    return Transform(left).combine(right);
}

Transform& operator*=(Transform& left, const Transform& right) { return left.combine(right); }

Vec2 operator*(const Transform& left, const Vec2& right) { return left.transform(right); }

bool operator==(const Transform& left, const Transform& right) {
    const float* a = left.matrix();
    const float* b = right.matrix();

    return ((a[0] == b[0]) && (a[1] == b[1]) && (a[3] == b[3]) && (a[4] == b[4]) &&
            (a[5] == b[5]) && (a[7] == b[7]) && (a[12] == b[12]) && (a[13] == b[13]) &&
            (a[15] == b[15]));
}

bool operator!=(const Transform& left, const Transform& right) { return !(left == right); }

}  // namespace gui
}  // namespace mahi