#include <Mahi/Gui/Transformable.hpp>
#include <cmath>

namespace mahi {
namespace gui {

Transformable::Transformable() :
    m_origin(0, 0),
    m_position(0, 0),
    m_rotation(0),
    m_scale(1, 1),
    m_transform(),
    m_transform_stale(true),
    m_inverse_transform(),
    m_inverse_transform_stale(true) {}

Transformable::~Transformable() {}

void Transformable::set_pos(float x, float y) {
    m_position.x              = x;
    m_position.y              = y;
    m_transform_stale         = true;
    m_inverse_transform_stale = true;
}

void Transformable::set_pos(const Vec2& position) { set_pos(position.x, position.y); }

void Transformable::set_rotation(float angle) {
    m_rotation = static_cast<float>(fmod(angle, 360));
    if (m_rotation < 0)
        m_rotation += 360.f;

    m_transform_stale         = true;
    m_inverse_transform_stale = true;
}

void Transformable::set_scale(float factorX, float factorY) {
    m_scale.x                 = factorX;
    m_scale.y                 = factorY;
    m_transform_stale         = true;
    m_inverse_transform_stale = true;
}

void Transformable::set_scale(const Vec2& factors) { set_scale(factors.x, factors.y); }

void Transformable::set_origin(float x, float y) {
    m_origin.x                = x;
    m_origin.y                = y;
    m_transform_stale         = true;
    m_inverse_transform_stale = true;
}

void Transformable::set_origin(const Vec2& origin) { set_origin(origin.x, origin.y); }

const Vec2& Transformable::pos() const { return m_position; }

float Transformable::rotation() const { return m_rotation; }

const Vec2& Transformable::scale() const { return m_scale; }

const Vec2& Transformable::origin() const { return m_origin; }

void Transformable::move(float offsetX, float offsetY) {
    set_pos(m_position.x + offsetX, m_position.y + offsetY);
}

void Transformable::move(const Vec2& offset) {
    set_pos(m_position.x + offset.x, m_position.y + offset.y);
}

void Transformable::rotate(float angle) { set_rotation(m_rotation + angle); }

void Transformable::scale(float factorX, float factorY) {
    set_scale(m_scale.x * factorX, m_scale.y * factorY);
}

void Transformable::scale(const Vec2& factor) {
    set_scale(m_scale.x * factor.x, m_scale.y * factor.y);
}

const Transform& Transformable::transform() const {
    // Recompute the combined transform if needed
    if (m_transform_stale) {
        float angle  = -m_rotation * 3.141592654f / 180.f;
        float cosine = static_cast<float>(std::cos(angle));
        float sine   = static_cast<float>(std::sin(angle));
        float sxc    = m_scale.x * cosine;
        float syc    = m_scale.y * cosine;
        float sxs    = m_scale.x * sine;
        float sys    = m_scale.y * sine;
        float tx     = -m_origin.x * sxc - m_origin.y * sys + m_position.x;
        float ty     = m_origin.x * sxs - m_origin.y * syc + m_position.y;

        m_transform       = Transform(sxc, sys, tx, -sxs, syc, ty, 0.f, 0.f, 1.f);
        m_transform_stale = false;
    }

    return m_transform;
}

const Transform& Transformable::inverse_transform() const {
    // Recompute the inverse transform if needed
    if (m_inverse_transform_stale) {
        m_inverse_transform       = transform().inverse();
        m_inverse_transform_stale = false;
    }

    return m_inverse_transform;
}

}  // namespace gui
}  // namespace mahi