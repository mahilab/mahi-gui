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
//
// Adapted from: SFML - Simple and Fast Multimedia Library (license at bottom)

#pragma once

#include <Mahi/Gui/Rect.hpp>
#include <Mahi/Gui/Vec2.hpp>

namespace mahi {
namespace gui {

/// Encapsulates a 3x3 transformation matrix
class Transform {
public:
    /// Default constructor
    Transform();
    /// Construct a transform from a 3x3 matrix
    Transform(float a00, float a01, float a02, 
              float a10, float a11, float a12, 
              float a20, float a21, float a22);
    /// Return the transform as a 4x4 matrix compatible with OpenGL
    const float* matrix() const;
    /// Return the inverse of the transform
    Transform inverse() const;
    /// Transform a 2D point
    Vec2 transform(float x, float y) const;
    /// Transform a 2D point
    Vec2 transform(const Vec2& point) const;
    /// Transform a rectangle
    Rect transform(const Rect& rectangle) const;
    /// Combine the current transform with another one
    Transform& combine(const Transform& transform);
    /// Combine the current transform with a translation
    Transform& translate(float x, float y);
    ///  Combine the current transform with a translation
    Transform& translate(const Vec2& offset);
    /// Combine the current transform with a rotation
    Transform& rotate(float angle);
    /// Combine the current transform with a rotation
    Transform& rotate(float angle, float centerX, float centerY);
    /// Combine the current transform with a rotation
    Transform& rotate(float angle, const Vec2& center);
    /// Combine the current transform with a scaling
    Transform& scale(float scaleX, float scaleY);
    /// Combine the current transform with a scaling
    Transform& scale(float scaleX, float scaleY, float centerX, float centerY);
    /// Combine the current transform with a scaling
    Transform& scale(const Vec2& factors);
    /// Combine the current transform with a scaling
    Transform& scale(const Vec2& factors, const Vec2& center);

public:
    static const Transform Identity;  ///< The identity transform
private:
    float m_matrix[16];  ///< 4x4 matrix defining the transformation
};

/// Equivalent to calling Transform(left).combine(right).
Transform operator*(const Transform& left, const Transform& right);
/// Equivalent to calling left.combine(right).
Transform& operator*=(Transform& left, const Transform& right);
/// Equivalent to calling left.transform(right).
Vec2 operator*(const Transform& left, const Vec2& right);
/// Performs an element-wise comparison of the elements of the
/// left transform with the elements of the right transform.
bool operator==(const Transform& left, const Transform& right);
/// Equivalent to !(left == right).
bool operator!=(const Transform& left, const Transform& right);

}  // namespace gui
}  // namespace mahi

// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2019 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this
// software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
