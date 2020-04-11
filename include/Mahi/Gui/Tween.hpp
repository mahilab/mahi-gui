// MIT License
//
// Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#pragma once

#include <Mahi/Gui/Color.hpp>

namespace mahi {
namespace gui {

namespace Tween {
/// Returns b instantly regardless of t.
template <typename T>
inline T Instant(const T& a, const T& b, float t);
/// Returns a until t = 1.0, then returns b.
template <typename T>
inline T Delayed(const T& a, const T& b, float t);
/// Transitions from a to b using linear interpolation
template <typename T>
inline T Linear(const T& a, const T& b, float t);
/// Transitions from a to b using 3rd order Hermite interpolation.
template <typename T>
inline T Smoothstep(const T& a, const T& b, float t);
/// Transitions from a to b using 5th order Hermite interpolation.
template <typename T>
inline T Smootherstep(const T& a, const T& b, float t);
/// Transitions from a to b using 7th order Hermite interpolation.
template <typename T>
inline T Smootheststep(const T& a, const T& b, float t);

/// Transitions from a to b using 2nd order polynomial interpolation.
namespace Quadratic {
template <typename T>
inline T In(const T& a, const T& b, float t);
template <typename T>
inline T Out(const T& a, const T& b, float t);
template <typename T>
inline T InOut(const T& a, const T& b, float t);
}  // namespace Quadratic

/// Transitions from a to b using 3rd order polynomial interpolation.
namespace Cubic {
template <typename T>
inline T In(const T& a, const T& b, float t);
template <typename T>
inline T Out(const T& a, const T& b, float t);
template <typename T>
inline T InOut(const T& a, const T& b, float t);
}  // namespace Cubic

/// Transitions from a to b using 4th order polynomial interpolation.
namespace Quartic {
template <typename T>
inline T In(const T& a, const T& b, float t);
template <typename T>
inline T Out(const T& a, const T& b, float t);
template <typename T>
inline T InOut(const T& a, const T& b, float t);
}  // namespace Quartic

/// Transitions from a to b using 5th order polynomial interpolation.
namespace Quintic {
template <typename T>
inline T In(const T& a, const T& b, float t);
template <typename T>
inline T Out(const T& a, const T& b, float t);
template <typename T>
inline T InOut(const T& a, const T& b, float t);
}  // namespace Quintic

/// Transitions from a to b using sinusoidal interpolation.
namespace Sinusoidal {
template <typename T>
inline T In(const T& a, const T& b, float t);
template <typename T>
inline T Out(const T& a, const T& b, float t);
template <typename T>
inline T InOut(const T& a, const T& b, float t);
}  // namespace Sinusoidal

/// Transitions from a to b using exponential interpolation.
namespace Exponential {
template <typename T>
inline T In(const T& a, const T& b, float t);
template <typename T>
inline T Out(const T& a, const T& b, float t);
template <typename T>
inline T InOut(const T& a, const T& b, float t);
}  // namespace Exponential

/// Transitions from a to b using circular interpolation.
namespace Circular {
template <typename T>
inline T In(const T& a, const T& b, float t);
template <typename T>
inline T Out(const T& a, const T& b, float t);
template <typename T>
inline T InOut(const T& a, const T& b, float t);
}  // namespace Circular

/// Transitions from a to b with an elastic effect.
namespace Elastic {
template <typename T>
inline T In(const T& a, const T& b, float t);
template <typename T>
inline T Out(const T& a, const T& b, float t);
template <typename T>
inline T InOut(const T& a, const T& b, float t);
}  // namespace Elastic

/// Transitions from a to b with an overshooting effect
namespace Back {
template <typename T>
inline T In(const T& a, const T& b, float t);
template <typename T>
inline T Out(const T& a, const T& b, float t);
template <typename T>
inline T InOut(const T& a, const T& b, float t);
}  // namespace Back

/// Transitions from a to b with a bouncing effect.
namespace Bounce {
template <typename T>
inline T In(const T& a, const T& b, float t);
template <typename T>
inline T Out(const T& a, const T& b, float t);
template <typename T>
inline T InOut(const T& a, const T& b, float t);
}  // namespace Bounce

}  // namespace Tween

}  // namespace gui
}  // namespace mahi

#include <Mahi/Gui/Detail/Tween.inl>
