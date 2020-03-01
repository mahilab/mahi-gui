#pragma once

#include <mahi/gui/Math.hpp>
#include <mahi/gui/Color.hpp>

namespace mahi::gui {

namespace Tween
{
    /// Returns b instantly regardless of t.
    template <typename T> inline T Instant(const T& a, const T& b, float t);
    /// Returns a until t = 1.0, then returns b.
    template <typename T> inline T Delayed(const T& a, const T& b, float t);
    /// Transitions from a to b using linear interpolation
    template <typename T> inline T Linear(const T& a, const T& b, float t);
    /// Transitions from a to b using 3rd order Hermite interpolation.
    template <typename T> inline T Smoothstep(const T& a, const T& b, float t);
    /// Transitions from a to b using 5th order Hermite interpolation.
    template <typename T> inline T Smootherstep(const T& a, const T& b, float t);
    /// Transitions from a to b using 7th order Hermite interpolation.
    template <typename T> inline T Smootheststep(const T& a, const T& b, float t);

    /// Transitions from a to b using 2nd order polynomial interpolation.
    namespace Quadratic
    {
        template <typename T> inline T In(const T& a, const T& b, float t);
        template <typename T> inline T Out(const T& a, const T& b, float t);
        template <typename T> inline T InOut(const T& a, const T& b, float t);
    }

    /// Transitions from a to b using 3rd order polynomial interpolation.
    namespace Cubic
    {
        template <typename T> inline T In(const T& a, const T& b, float t);
        template <typename T> inline T Out(const T& a, const T& b, float t);
        template <typename T> inline T InOut(const T& a, const T& b, float t);
    }

    /// Transitions from a to b using 4th order polynomial interpolation.
    namespace Quartic
    {
        template <typename T> inline T In(const T& a, const T& b, float t);
        template <typename T> inline T Out(const T& a, const T& b, float t);
        template <typename T> inline T InOut(const T& a, const T& b, float t);
    }

    /// Transitions from a to b using 5th order polynomial interpolation.
    namespace Quintic
    {
        template <typename T> inline T In(const T& a, const T& b, float t);
        template <typename T> inline T Out(const T& a, const T& b, float t);
        template <typename T> inline T InOut(const T& a, const T& b, float t);
    }

    /// Transitions from a to b using sinusoidal interpolation.
    namespace Sinusoidal
    {
        template <typename T> inline T In(const T& a, const T& b, float t);
        template <typename T> inline T Out(const T& a, const T& b, float t);
        template <typename T> inline T InOut(const T& a, const T& b, float t);
    }

    /// Transitions from a to b using exponential interpolation.
    namespace Exponential
    {
        template <typename T> inline T In(const T& a, const T& b, float t);
        template <typename T> inline T Out(const T& a, const T& b, float t);
        template <typename T> inline T InOut(const T& a, const T& b, float t);
    }

    /// Transitions from a to b using circular interpolation.
    namespace Circular
    {
        template <typename T> inline T In(const T& a, const T& b, float t);
        template <typename T> inline T Out(const T& a, const T& b, float t);
        template <typename T> inline T InOut(const T& a, const T& b, float t);
    }

    /// Transitions from a to b with an elastic effect.
    namespace Elastic
    {
        template <typename T> inline T In(const T& a, const T& b, float t);
        template <typename T> inline T Out(const T& a, const T& b, float t);
        template <typename T> inline T InOut(const T& a, const T& b, float t);
    }

    /// Transitions from a to b with an overshooting effect
    namespace Back
    {
        template <typename T> inline T In(const T& a, const T& b, float t);
        template <typename T> inline T Out(const T& a, const T& b, float t);
        template <typename T> inline T InOut(const T& a, const T& b, float t);
    }

    /// Transitions from a to b with a bouncing effect.
    namespace Bounce
    {
        template <typename T> inline T In(const T& a, const T& b, float t);
        template <typename T> inline T Out(const T& a, const T& b, float t);
        template <typename T> inline T InOut(const T& a, const T& b, float t);
    }

} // namespace Tween

} // namespace mahi::gui

#include <mahi/gui/Detail/Tween.inl>
