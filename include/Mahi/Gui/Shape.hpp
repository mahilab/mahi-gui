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
#include <Mahi/Gui/Transform.hpp>
#include <vector>

namespace mahi {
namespace gui {

/// Encapsulates an advanced vector graphics object
class Shape {
public:
    /// Mode by which to query shape information
    enum QueryMode {
        Points,   ///< queried against points
        Vertices  ///< queried against vertices
    };

public:
    /// Default constructor
    Shape(std::size_t pointCount = 0);
    /// Default destructor
    virtual ~Shape();
    /// Sets the number of points in the Shape
    void set_point_count(std::size_t count);
    /// Gets the number of points in the Shape
    std::size_t point_count() const;
    /// Sets the position of a point
    void set_point(std::size_t index, Vec2 position);
    /// Sets the position of a point
    void set_point(std::size_t index, float x, float y);
    /// Sets the positions of all points in a shape and resets the point count
    void set_points(const std::vector<Vec2>& points);
    /// Gets the position of a point
    Vec2 point(std::size_t index) const;
    /// Gets the positions of all points
    const std::vector<Vec2>& points() const;
    /// Adds a new point and increments the point count
    void push_back(Vec2 position);
    /// Adds a new point and increments the point count
    void push_back(float x, float y);
    /// Translates all points
    void move(float offsetX, float offsetY);
    /// Translate all points
    void move(const Vec2& offset);
    /// Rotates all points about the origin
    void rotate(float angle);
    /// Rotates all points about a single point
    void rotate(float angle, const Vec2& center);
    /// Rotates all points about a single point
    void rotate(float angle, float cx, float cy);
    /// Translates all points
    void scale(float scaleX, float scaleY);
    /// Translate all points
    void scale(const Vec2& scale);
    /// Transform all points
    void transform(const Transform& matrix);
    /// Sets the radius associated with a point
    void set_radius(std::size_t index, float radius, std::size_t smoothness = 10);
    /// Gets the radius associated with a point
    float radius(std::size_t index) const;
    /// Sets the radii for all points to a single radius
    void set_radii(float radius, std::size_t smoothness = 10);
    /// Sets the radii for all points
    void set_radii(const std::vector<float>& radii);
    /// Gets the radii for all points
    const std::vector<float>& radii() const;
    /// Gets the number of vertices making up the Shape's outer contour after
    /// all radii have been applied
    std::size_t vertex_count() const;
    /// Gets the vertices making up the Shape's outer contour after all radii have been applied.
    const std::vector<Vec2>& vertices() const;
    /// Permanently applies all radii (i.e. converts vertices to points)
    void apply_radii();
    /// Sets the number of holes in the Shape
    void set_hole_count(std::size_t count);
    /// Gets the number of holes in the Shape
    std::size_t hole_count() const;
    /// Sets the Shape of a hole
    void set_hole(std::size_t index, const Shape& hole);
    /// Gets the Shape of a hole
    const Shape& hole(std::size_t index) const;
    /// Returns entire list of holes
    std::vector<Shape>& holes();
    /// Adds a new hole and increments to hole count
    void push_back_hole(const Shape& hole);
    /// Gets the local bounding rectangle of the Shape
    Rect bounds(QueryMode mode = Points) const;
    /// Tests if a point is inside of a Shape
    bool contains(const Vec2& point, QueryMode mode = Points) const;
    /// Returns area of shape including radii and holes
    float area(QueryMode mode = Points) const;
    /// Returns true if the Shape is convex, false if concave
    bool is_convex() const;

private:
    void update_if_stale() const;
    void update_vertices() const;
    void update_bounds() const;

private:
    std::vector<Vec2>         m_points;
    std::vector<float>        m_radii;
    std::vector<std::size_t>  m_smoothness;
    std::vector<Shape>        m_holes;
    mutable std::vector<Vec2> m_vertices;
    mutable Rect              m_pointsBounds;
    mutable Rect              m_verticesBounds;
    mutable bool              m_stale;
};

//============================================================================
// SHAPE MAKERS
//============================================================================

/// Offset type options
enum OffsetType { Miter, Round, Square };

/// Creates a new Shape that is offset (positive or negative) from another Shape
Shape offset_shape(const Shape& shape, float offset, OffsetType type = Miter);

/// Clipping type options
enum ClipType {
    Intersection,  ///< region where subject and clip are both filled
    Union,         ///< region where either subject or clip are filled
    Difference,    ///< region where subject is filled except where clip is filled
    Exclusion      ///< region where either subject or clip is filled, but not where both are filled
};

/// Returns a new shape that is the combination of subject and clip according to ClipType type
std::vector<Shape> clip_shapes(const Shape& subject, const Shape& clip, ClipType type);

enum PolyParam {
    CircumscribedRadius,  /// value interpreted as radius of circle that intsects vertices
    InscribedRadius,      /// value interpreted as radius of circle that is tangent to sides
    SideLength            /// value interpreted as side length
};

/// Makes a regular normal polyon shape
inline Shape make_polygon_shape(std::size_t sides, float value,
                                PolyParam param = CircumscribedRadius) {
    float r = value;
    if (param == InscribedRadius)
        r = value / std::cos((float)util::PI / sides);
    else if (param == SideLength)
        r = value / (2 * std::sin((float)util::PI / sides));
    Shape shape(sides);
    float angleIncrement = 2.0f * (float)util::PI / sides;
    for (std::size_t i = 0; i < sides; i++) {
        float angle = i * angleIncrement - 0.5f * (float)util::PI;
        shape.set_point(i, std::cos(angle) * r, std::sin(angle) * r);
    }
    return shape;
}

/// Makes a centered rectangel shape
inline Shape make_rectangle_shape(float width, float height) {
    Shape shape(4);
    float halfWidth  = 0.5f * width;
    float halfHeight = 0.5f * height;
    shape.set_point(0, -halfWidth, -halfHeight);
    shape.set_point(1, halfWidth, -halfHeight);
    shape.set_point(2, halfWidth, halfHeight);
    shape.set_point(3, -halfWidth, halfHeight);
    return shape;
}

/// Makes a normal star shape
inline Shape make_star_shape(std::size_t points, float r1, float r2) {
    Shape shape(points * 2);
    float angleIncrement = (float)util::PI / points;
    float offset         = (float)util::PI * 0.5f;
    for (std::size_t i = 0; i < points; i++) {
        float angleA = 2 * i * angleIncrement + offset;
        float angleB = (2 * i + 1) * angleIncrement + offset;
        shape.set_point(2 * i, r1 * std::cos(angleA), r1 * std::sin(angleA));
        shape.set_point(2 * i + 1, r2 * std::cos(angleB), r2 * std::sin(angleB));
    }
    return shape;
}

/// Makes a cross shape
inline Shape make_cross_shape(float width, float height, float thickness) {
    float halfWidth     = 0.5f * width;
    float halfHeight    = 0.5f * height;
    float halfThickness = 0.5f * thickness;
    Shape shape(12);
    shape.set_point(0, -halfThickness, halfHeight);
    shape.set_point(1, halfThickness, halfHeight);
    shape.set_point(2, halfThickness, halfThickness);
    shape.set_point(3, halfWidth, halfThickness);
    shape.set_point(4, halfWidth, -halfThickness);
    shape.set_point(5, halfThickness, -halfThickness);
    shape.set_point(6, halfThickness, -halfHeight);
    shape.set_point(7, -halfThickness, -halfHeight);
    shape.set_point(8, -halfThickness, -halfThickness);
    shape.set_point(9, -halfWidth, -halfThickness);
    shape.set_point(10, -halfWidth, halfThickness);
    shape.set_point(11, -halfThickness, halfThickness);
    return shape;
}

// TODO: Import more make_xxx_shape functions from carnot and ShapeRenderer

}  // namespace gui
}  // namespace mahi
