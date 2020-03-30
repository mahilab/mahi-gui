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

namespace mahi {
namespace gui {

/// Creates an NVG path from a Shape
inline void path_shape(NVGcontext* vg, const Shape& shape) {
    if (shape.vertex_count() < 3)
        return;
    const auto& vertices = shape.vertices();
    nvgMoveTo(vg, vertices[0].x, vertices[0].y);
    for (unsigned int i = 1; i < vertices.size(); ++i) {
        nvgLineTo(vg, vertices[i].x, vertices[i].y);
    }
    nvgClosePath(vg);
}

/// Fills a Shape with NVG
inline void fill_shape(NVGcontext* vg, const Shape& shape, const Color& c) {
    nvgBeginPath(vg);
    path_shape(vg, shape);
    nvgFillColor(vg, c);
    nvgFill(vg);
}

/// Strokes a Shape with NVG
inline void stroke_shape(NVGcontext* vg, const Shape& shape, float width, const Color& c) {
    nvgBeginPath(vg);
    path_shape(vg, shape);
    nvgStrokeWidth(vg,width);
    nvgStrokeColor(vg, nvgRGBAf(c.r,c.g,c.b,c.a));
    nvgStroke(vg);
}

/// Draws an SVG file from a NSVGimage, created with nsvgParseFromFile
void draw_svg(NVGcontext* vg, NSVGimage* svg);

} // namespace gui
} // namespace mahi