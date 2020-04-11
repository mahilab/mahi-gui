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
#include <Mahi/Gui/Transform.hpp>

namespace mahi {
namespace gui {

class Transformable {
public:
    /// Default constructor
    Transformable();
    // Virtual destructor
    virtual ~Transformable();
    /// Set the position of the object
    void set_pos(float x, float y);
    /// Set the position of the object
    void set_pos(const Vec2& position);
    /// Set the orientation of the object
    void set_rotation(float angle);
    /// Set the scale factors of the object
    void set_scale(float factorX, float factorY);
    /// Set the scale factors of the object
    void set_scale(const Vec2& factors);
    /// Set the local origin of the object
    void set_origin(float x, float y);
    /// Set the local origin of the object
    void set_origin(const Vec2& origin);
    /// Get the position of the object
    const Vec2& pos() const;
    /// Get the orientation of the object
    float rotation() const;
    /// Get the current scale of the object
    const Vec2& scale() const;
    /// Get the local origin of the object
    const Vec2& origin() const;
    /// Move the object by a given offset
    void move(float offsetX, float offsetY);
    /// Move the object by a given offset
    void move(const Vec2& offset);
    /// Rotate the object
    void rotate(float angle);
    /// Scale the object
    void scale(float factorX, float factorY);
    /// Scale the object
    void scale(const Vec2& factor);
    /// Get the combined transform of the object
    const Transform& transform() const;
    /// Get the inverse of the combined transform of the object
    const Transform& inverse_transform() const;

private:
    Vec2              m_origin;           ///< Origin of translation/rotation/scaling of the object
    Vec2              m_position;         ///< Position of the object in the 2D world
    float             m_rotation;         ///< Orientation of the object, in degrees
    Vec2              m_scale;            ///< Scale of the object
    mutable Transform m_transform;        ///< Combined transformation of the object
    mutable bool      m_transform_stale;  ///< Does the transform need to be recomputed?
    mutable Transform m_inverse_transform;        ///< Combined transformation of the object
    mutable bool      m_inverse_transform_stale;  ///< Does the transform need to be recomputed?
};

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