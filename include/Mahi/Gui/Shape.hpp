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
        Points,  ///< queried against points
        Vertices ///< queried against vertices
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
    /// Rotates all points about a particular point
    void rotate(float angle);
    /// Translates all points
    void scale(float scaleX, float scaleY);
    /// Translate all points
    void scale(const Vec2& scale);
    /// Transform all points
    virtual void transform(const Transform& matrix);
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
    /// Gets the vertices making up the Shape's outer contour after all radii
    // have been applied.
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

public:

    /// Offset type options
    enum OffsetType {
        Miter,
        Round,
        Square
    };

    /// Clipping type options
    enum ClipType {
        Intersection, ///< region where subject and clip are filled
        Union,        ///< region where either subject or clip are filled
        Difference,   ///< region where subject is filled except where clip is filled
        Exclusion     ///< region where either subject or clip is filled, but not where both are filled
    };

    static Shape offset_shape(const Shape& shape, float offset, OffsetType type = Miter);
    static std::vector<Shape> clip_shapes(const Shape& subject, const Shape& clip, ClipType type);

private:

    void update_if_stale() const;
    void update_vertices() const;
    void update_bounds() const;

private:
    std::vector<Vec2> m_points;
    std::vector<float> m_radii;
    std::vector<std::size_t> m_smoothness;
    std::vector<Shape> m_holes;
    mutable std::vector<Vec2> m_vertices;
    mutable Rect m_pointsBounds;
    mutable Rect m_verticesBounds;
    mutable bool m_stale;
};

} // namespace gui
} // namespace mahi
