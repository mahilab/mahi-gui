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
    inline Vec2 pos() const { return Vec2(left, top); }

    /// Returns the Rect size (width,height)
    inline Vec2 size() const { return Vec2(width, height); }

    /// Returns the Rect top left
    inline Vec2 tl() const { return Vec2(left, top); }

    /// Returns the Rect top right
    inline Vec2 tr() const { return Vec2(left + width, top); }

    /// Returns the Rect bottom left
    inline Vec2 bl() const { return Vec2(left, top + height); }

    /// Returns the Rect bottom right
    inline Vec2 br() const { return Vec2(left + width, top + height); }

    /// Returns the Rect center
    inline Vec2 center() { return Vec2(left + width * 0.5f, top + height * 0.5f); }

    /// Returns true if the Rect contains the point p
    inline bool contains(const Vec2& p) const {
        return (p.x >= left) && (p.x < left + width) && (p.y >= top) && (p.y < top + height);
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