

namespace mahi {
namespace gui {

namespace Tween {

//==============================================================================
// BASIC TWEENS
//==============================================================================

template <typename T>
inline T Instant(const T& a, const T& b, float t) {
    return b;
}

template <typename T>
inline T Delayed(const T& a, const T& b, float t) {
    if (t == 1.0f)
        return b;
}

template <typename T>
inline T Linear(const T& a, const T& b, float t) {
    return a + (b - a) * t;
}

template <>
inline Color Linear(const Color& a, const Color& b, float t) {
    return Color {
        Linear(a.r, b.r, t),
        Linear(a.g, b.g, t),
        Linear(a.b, b.b, t),
        Linear(a.a, b.a, t)
    };
}

template <>
inline HSV Linear(const HSV& a, const HSV& b, float t) {
    return HSV {
        Linear(a.h, b.h, t),
        Linear(a.s, b.s, t),
        Linear(a.v, b.v, t),
        Linear(a.a, b.a, t)
    };
}

template <typename T>
inline std::vector<T> Linear(const std::vector<T>& a, const std::vector<T>& b, float t) {
    std::size_t n = std::min(a.size(), b.size());
    std::vector<T> out(n);
    for (std::size_t i = 0; i < n; ++i) {
        out[i] = Linear(a[i], b[i], t);
    }
    return out;
}

template <typename T>
inline T Smoothstep(const T& a, const T& b, float t) {
    t = t * t * (3.0f - 2.0f * t);
    return Linear(a, b, t);
}

template <typename T>
inline T Smootherstep(const T& a, const T& b, float t) {
    t = t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    return Linear(a, b, t);
}

template <typename T>
inline T Smootheststep(const T& a, const T& b, float t) {
    t = t * t * t * t * (t * (t * (t * -20.0f + 70.0f) - 84.0f) + 35.0f);
    return Linear(a, b, t);
}

//==============================================================================
// POLYNOMIAL TWEENS
//==============================================================================

namespace Quadratic {
template <typename T>
inline T In(const T& a, const T& b, float t) {
    t = t * t;
    return Linear(a, b, t);
}

template <typename T>
inline T Out(const T& a, const T& b, float t) {
    t = t * (2.0f - t);
    return Linear(a, b, t);
}

template <typename T>
inline T InOut(const T& a, const T& b, float t) {
    t *= 2.0f;
    if (t < 1.0f)
        t = 0.5f * t * t;
    else {
        t -= 1.0f;
        t = -0.5f * (t * (t - 2.0f) - 1.0f);
    }
    return Linear(a, b, t);
}

};  // namespace Quadratic

namespace Cubic {
template <typename T>
inline T In(const T& a, const T& b, float t) {
    t = t * t * t;
    return Linear(a, b, t);
}

template <typename T>
inline T Out(const T& a, const T& b, float t) {
    t -= 1.0f;
    t = (t * t * t + 1.0f);
    return Linear(a, b, t);
}

template <typename T>
inline T InOut(const T& a, const T& b, float t) {
    t *= 2.0f;
    if (t < 1.0f)
        t = 0.5f * t * t * t;
    else {
        t -= 2.0f;
        t = 0.5f * (t * t * t + 2.0f);
    }
    return Linear(a, b, t);
}

};  // namespace Cubic

namespace Quartic {
template <typename T>
inline T In(const T& a, const T& b, float t) {
    t = t * t * t * t;
    return Linear(a, b, t);
}

template <typename T>
inline T Out(const T& a, const T& b, float t) {
    t -= 1.0f;
    t = -(t * t * t * t - 1.0f);
    return Linear(a, b, t);
}

template <typename T>
inline T InOut(const T& a, const T& b, float t) {
    t *= 2.0f;
    if (t < 1.0f)
        t = 0.5f * t * t * t * t;
    else {
        t -= 2.0f;
        t = -0.5f * (t * t * t * t - 2.0f);
    }
    return Linear(a, b, t);
}

};  // namespace Quartic

namespace Quintic {
template <typename T>
inline T In(const T& a, const T& b, float t) {
    t = t * t * t * t * t;
    return Linear(a, b, t);
}

template <typename T>
inline T Out(const T& a, const T& b, float t) {
    t -= 1.0f;
    t = (t * t * t * t * t + 1.0f);
    return Linear(a, b, t);
}

template <typename T>
inline T InOut(const T& a, const T& b, float t) {
    t *= 2.0f;
    if (t < 1.0f)
        t = 0.5f * t * t * t * t * t;
    else {
        t -= 2.0f;
        t = 0.5f * (t * t * t * t * t + 2.0f);
    }
    return Linear(a, b, t);
}

};  // namespace Quintic

//==============================================================================
// MATHEMATICAL TWEENS
//==============================================================================

namespace Sinusoidal {
template <typename T>
inline T In(const T& a, const T& b, float t) {
    t = 1.0f - std::cos(t * mahi::util::HALFPI);
    return Linear(a, b, t);
}

template <typename T>
inline T Out(const T& a, const T& b, float t) {
    t = std::sin(t * mahi::util::HALFPI);
    return Linear(a, b, t);
}

template <typename T>
inline T InOut(const T& a, const T& b, float t) {
    t = -0.5f * (std::cos(mahi::util::PI * t) - 1.0f);
    return Linear(a, b, t);
}

};  // namespace Sinusoidal

namespace Exponential {
template <typename T>
inline T In(const T& a, const T& b, float t) {
    t = std::pow(2.0f, 10.0f * (t - 1.0f));
    return Linear(a, b, t);
}

template <typename T>
inline T Out(const T& a, const T& b, float t) {
    t = (-std::pow(2.0f, -10.0f * t) + 1.0f);
    return Linear(a, b, t);
}

template <typename T>
inline T InOut(const T& a, const T& b, float t) {
    t *= 2.0f;
    if (t < 1.0f)
        t = 0.5f * std::pow(2.0f, 10.0f * (t - 1.0f));
    else {
        t -= 1.0f;
        t = 0.5f * (-std::pow(2.0f, -10.0f * t) + 2.0f);
    }
    return Linear(a, b, t);
}

};  // namespace Exponential

namespace Circular {
template <typename T>
inline T In(const T& a, const T& b, float t) {
    t = (1.0f - std::sqrt(1.0f - t * t));
    return Linear(a, b, t);
}

template <typename T>
inline T Out(const T& a, const T& b, float t) {
    t -= 1.0f;
    t = std::sqrt(1.0f - t * t);
    return Linear(a, b, t);
}

template <typename T>
inline T InOut(const T& a, const T& b, float t) {
    t *= 2.0f;
    if (t < 1.0f)
        t = -0.5f * (std::sqrt(1.0f - t * t) - 1.0f);
    else {
        t -= 2.0f;
        t = 0.5f * (std::sqrt(1.0f - t * t) + 1.0f);
    }
    return Linear(a, b, t);
}

};  // namespace Circular

//==============================================================================
// EFFECT TWEENS
//==============================================================================

namespace Elastic {
template <typename T>
inline T In(const T& a, const T& b, float t) {
    if (t == 0.0f) {
        return a;
    }
    if (t == 1.0f) {
        return b;
    }
    t -= 1.0f;
    t = (-std::pow(2.0f, 10.0f * t) * std::sin((t - 0.1f) * (2.0f * mahi::util::PI) * 2.5f));
    return Linear(a, b, t);
}

template <typename T>
inline T Out(const T& a, const T& b, float t) {
    if (t == 0.0f) {
        return a;
    }
    if (t == 1.0f) {
        return b;
    }
    t =  (std::pow(2.0f, -10.0f * t) * std::sin((t - 0.1f) * (2.0f * mahi::util::PI) * 2.5f) + 1.0f);
    return Linear(a, b, t);
}

template <typename T>
inline T InOut(const T& a, const T& b, float t) {
    t *= 2.0f;
    if (t < 1.0f) {
        t -= 1.0f;
        t = (-0.5f * std::pow(2.0f, 10.0f * t) * std::sin((t - 0.1f) * (2.0f * mahi::util::PI) * 2.5f));
    }
    else {
        t -= 1.0f;
        t = (std::pow(2.0f, -10.0f * t) * std::sin((t - 0.1f) * (2.0f * mahi::util::PI) * 2.5f) * 0.5f + 1.0f);
    }
    return Linear(a, b, t);
}

};  // namespace Elastic

namespace Back {
static float s  = 1.70158f;
static float s2 = 2.5949095f;

template <typename T>
inline T In(const T& a, const T& b, float t) {
    t = (t * t * ((s + 1.0f) * t - s));
    return Linear(a, b, t);
}

template <typename T>
inline T Out(const T& a, const T& b, float t) {
    t -= 1.0f;
    t = (t * t * ((s + 1.0f) * t + s) + 1.0f);
    return Linear(a, b, t);
}

template <typename T>
inline T InOut(const T& a, const T& b, float t) {
    t *= 2.0f;
    if (t < 1.0f) {
        t = (0.5f * (t * t * ((s2 + 1.0f) * t - s2)));
    }
    else {
        t -= 2.0f;
        t = (0.5f * (t * t * ((s2 + 1.0f) * t + s2) + 2.0f));
    }
    return Linear(a, b, t);
}

};  // namespace Back

namespace Bounce {

namespace detail {
inline float Out(float t) {
    if (t < (1.0f / 2.75f)) {
        return 7.5625f * t * t;
    } else if (t < (2.0f / 2.75f)) {
        t -= (1.5f / 2.75f);
        return 7.5625f * t * t + 0.75f;
    } else if (t < (2.5f / 2.75f)) {
        t -= (2.25f / 2.75f);
        return 7.5625f * t * t + 0.9375f;
    } else {
        t -= (2.625f / 2.75f);
        return 7.5625f * t * t + 0.984375f;
    }
}

inline float In(float t) {
    return 1.0f - Out(1.0f - t);
}

inline float InOut(float t) {
    if (t < 0.5f)
        return In(t * 2.0f) * 0.5f;
    return Out(t * 2.0f - 1.0f) * 0.5f + 0.5f;
}
}  // namespace detail

template <typename T>
inline T In(const T& a, const T& b, float t) {
    t = detail::In(t);
    return Linear(a, b, t);
}

template <typename T>
inline T Out(const T& a, const T& b, float t) {
    t = detail::Out(t);
    return Linear(a, b, t);
}

template <typename T>
inline T InOut(const T& a, const T& b, float t) {
    t = detail::InOut(t);
    return Linear(a, b, t);
}

};  // namespace Bounce

}  // namespace Tween

}  // namespace gui
}  // namespace mahi
