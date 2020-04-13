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
#include <nanovg.h>
#include <nanosvg.h>
#include <Mahi/Gui/Shape.hpp>
#include <Mahi/Gui/Color.hpp>
#include <Mahi/Gui/Transform.hpp>

namespace mahi {
namespace gui {

/// Creates an NVG path from a Shape
inline void nvgShape(NVGcontext* vg, const Shape& shape) {
    if (shape.vertex_count() < 3)
        return;
    const auto& vertices = shape.vertices();
    nvgMoveTo(vg, vertices[0].x, vertices[0].y);
    for (unsigned int i = 1; i < vertices.size(); ++i) {
        nvgLineTo(vg, vertices[i].x, vertices[i].y);
    }
    nvgClosePath(vg);
}

/// Creates an NVG path from a Shape that contains holes
inline void nvgShapeWithHoles(NVGcontext* vg, const Shape& shape) {
    nvgShape(vg, shape);
    for (std::size_t i = 0; i < shape.hole_count(); ++i) {
        nvgShape(vg, shape.hole(i));
        nvgPathWinding(vg, NVG_HOLE);
    }
}

/// Fills a Shape with NVG (will render holes if they  exists)
inline void nvgFillShape(NVGcontext* vg, const Shape& shape, const Color& c) {
    nvgBeginPath(vg);
    nvgShapeWithHoles(vg, shape);
    nvgFillColor(vg, c);
    nvgFill(vg);
}

/// Strokes a Shape with NVG
inline void nvgStrokeShape(NVGcontext* vg, const Shape& shape, float width, const Color& c) {
    nvgBeginPath(vg);
    nvgShapeWithHoles(vg, shape);
    nvgStrokeWidth(vg,width);
    nvgStrokeColor(vg, nvgRGBAf(c.r,c.g,c.b,c.a));
    nvgStroke(vg);
}

/// Premultiplies current coordinate system by specified Transformation matrix.
inline void nvgTransform(NVGcontext* vg, const Transform& transform) {
    auto m = transform.matrix();
    nvgTransform(vg, m[0], m[1], m[4], m[5], m[12], m[13]);
}

/// Draws an SVG file from a NSVGimage, created with nsvgParseFromFile
void nvgDrawSvg(NVGcontext* vg, NSVGimage* svg);

/// Parses SVG from string
NSVGimage* nsvgParseFromString(const std::string& str, const char* units, float dpi);


} // namespace gui
} // namespace mahi