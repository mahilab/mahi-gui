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
    Sequence(T (*tweenFunc)(const T&, const T&, float) = Tween::Linear);

    /// Sets a keyframe value in the sequence
    T& operator[](float t);

    /// Retrieves an interpolated value
    T operator()(float t) const;

    /// Sets the tweening function to be used
    void setTween(T (*tweenFunc)(const T&, const T&, float));
    
    /// Gets keyframe stops and values
    void getKeys(std::vector<float>& stopsOut, std::vector<T>& valuesOut) const;

private:

    std::map<float, T> m_keys;                    ///< keyframes
    T (*m_tweenFunc)(const T&, const T&, float);  ///< tweening function
};

//==============================================================================
// Template Definitions
//==============================================================================

template <typename T>
Sequence<T>::Sequence(T (*tweenFunc)(const T&, const T&, float)) :
    m_keys(),
    m_tweenFunc(tweenFunc)
{}

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
    t = (t - a->first) / (b->first - a->first);
    return m_tweenFunc(a->second, b->second, t);
}

template <typename T>
void Sequence<T>::setTween(T (*tweenFunc)(const T&, const T&, float)) {
    m_tweenFunc = tweenFunc;
}

template <typename T>
void Sequence<T>::getKeys(std::vector<float>& stopsOut, std::vector<T>& valuesOut) const {
    stopsOut.clear(); stopsOut.reserve(m_keys.size());
    valuesOut.clear(); valuesOut.reserve(m_keys.size());   
    for (auto it = m_keys.begin(); it != m_keys.end(); ++it) {
        stopsOut.push_back(it->first);
        valuesOut.push_back(it->second);
    }
    
}

} // namespace gui
} // namespace mahi
