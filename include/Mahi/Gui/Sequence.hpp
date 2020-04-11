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

#include <Mahi/Gui/Tween.hpp>
#include <map>

namespace mahi {
namespace gui {

/// Sequence class
template <typename T>
class Sequence {
public:
    /// Default constructor
    Sequence(T (*tween_func)(const T&, const T&, float) = Tween::Linear);

    /// Sets a keyframe value in the sequence
    T& operator[](float t);

    /// Retrieves an interpolated value
    T operator()(float t) const;

    /// Sets the tweening function to be used
    void set_tween(T (*tween_func)(const T&, const T&, float));

    /// Gets keyframe stops and values
    void get_keys(std::vector<float>& stops_out, std::vector<T>& values_out) const;

private:
    std::map<float, T> m_keys;                     ///< keyframes
    T (*m_tween_func)(const T&, const T&, float);  ///< tweening function
};

//==============================================================================
// Template Definitions
//==============================================================================

template <typename T>
Sequence<T>::Sequence(T (*tween_func)(const T&, const T&, float)) :
    m_keys(), m_tween_func(tween_func) {}

template <typename T>
T& Sequence<T>::operator[](float t) {
    assert(t >= 0.0f && t <= 1.0f);
    return m_keys[t];
}

template <typename T>
T Sequence<T>::operator()(float t) const {
    assert(m_keys.count(0.0f) && m_keys.count(1.0f));
    assert(t >= 0.0f && t <= 1.0f);
    auto b = m_keys.lower_bound(t);
    if (b->first == t)
        return b->second;
    auto a = std::prev(b);
    t      = (t - a->first) / (b->first - a->first);
    return m_tween_func(a->second, b->second, t);
}

template <typename T>
void Sequence<T>::set_tween(T (*tween_func)(const T&, const T&, float)) {
    m_tween_func = tween_func;
}

template <typename T>
void Sequence<T>::get_keys(std::vector<float>& stops_out, std::vector<T>& values_out) const {
    stops_out.clear();
    stops_out.reserve(m_keys.size());
    values_out.clear();
    values_out.reserve(m_keys.size());
    for (auto it = m_keys.begin(); it != m_keys.end(); ++it) {
        stops_out.push_back(it->first);
        values_out.push_back(it->second);
    }
}

}  // namespace gui
}  // namespace mahi
