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

#include <Mahi/Gui/Vec2.hpp>
#include <algorithm>

namespace mahi {
namespace gui {

/// 2D Axis-aligned Rectangle
struct Rect {
    /// Constructor
    Rect() : left(0), top(0), width(0), height(0) {}

    /// Constructor
    Rect(float left, float top, float width, float height) :
        left(left), top(top), width(width), height(height) {}

    /// Constructor
    Rect(const Vec2& position, const Vec2& size) :
        left(position.x), top(position.y), width(size.x), height(size.y) {}

    /// Returns the Rect position (left,top)
    inline Vec2 position() const { return Vec2(left, top); }

    /// Returns the REct size (width,height)
    inline Vec2 size() const { return Vec2(width, height); }

    /// Returns true if the Rect contains the point p
    bool contains(const Vec2& p) const {
        float min_x = std::min(left, left + width);
        float max_x = std::max(left, left + width);
        float min_y = std::min(top, top + height);
        float max_y = std::max(top, top + height);
        return (p.x >= min_x) && (p.x < max_x) && (p.y >= min_y) && (p.y < max_y);
    }

    float left;
    float top;
    float width;
    float height;
};

/// Compares if two Rects are the same
inline bool operator==(const Rect& left, const Rect& right) {
    return (left.left == right.left) && (left.width == right.width) && (left.top == right.top) &&
           (left.height == right.height);
}

/// Compares if two Rects are not the same
inline bool operator!=(const Rect& left, const Rect& right) { return !(left == right); }

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