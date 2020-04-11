#include <Mahi/Gui/Shape.hpp>
#include <iostream>
#include <limits>
#include "clipper.hpp"

#define CLIPPER_PREC     1000.0f
#define INV_CLIPPER_PREC 0.001f

namespace mahi {
namespace gui {

namespace {

ClipperLib::Path to_clipper(const std::vector<Vec2>& vec) {
    ClipperLib::Path clipper;
    clipper.resize(vec.size());
    for (std::size_t i = 0; i < vec.size(); ++i) {
        clipper[i] = ClipperLib::IntPoint(static_cast<ClipperLib::cInt>(vec[i].x * CLIPPER_PREC),
                                          static_cast<ClipperLib::cInt>(vec[i].y * CLIPPER_PREC));
    }
    return clipper;
}

std::vector<Vec2> from_clipper(const ClipperLib::Path& clipper) {
    std::vector<Vec2> vec(clipper.size());
    for (std::size_t j = 0; j < clipper.size(); ++j) {
        vec[j] = Vec2(static_cast<float>(clipper[j].X) * INV_CLIPPER_PREC,
                      static_cast<float>(clipper[j].Y) * INV_CLIPPER_PREC);
    }
    return vec;
}

}  // namespace

//==============================================================================
// PUBLIC FUNCTIONS
//==============================================================================

Shape::Shape(std::size_t pointCount) : m_holes(0), m_stale(true) { set_point_count(pointCount); }

Shape::~Shape() {}

void Shape::set_point_count(std::size_t count) {
    m_points.resize(count);
    m_radii.resize(count);
    m_smoothness.resize(count);
    m_stale = true;
}

std::size_t Shape::point_count() const { return m_points.size(); }

void Shape::set_point(std::size_t index, Vec2 position) {
    m_points[index] = position;
    m_stale         = true;
}

void Shape::set_point(std::size_t index, float x, float y) { set_point(index, Vec2(x, y)); }

void Shape::set_points(const std::vector<Vec2>& points) {
    set_point_count(points.size());
    m_points = points;
}

Vec2 Shape::point(std::size_t index) const { return m_points[index]; }

const std::vector<Vec2>& Shape::points() const { return m_points; }

void Shape::push_back(Vec2 position) {
    m_points.push_back(position);
    m_radii.push_back(float());
    m_smoothness.push_back(std::size_t());
    m_stale = true;
}

void Shape::push_back(float x, float y) { push_back(Vec2(x, y)); }

void Shape::move(float x, float y) {
    Transform matrix = Transform::Identity;
    matrix.translate(x, y);
    transform(matrix);
}

void Shape::move(const Vec2& offset) {
    Transform matrix = Transform::Identity;
    matrix.translate(offset.x, offset.y);
    transform(matrix);
}

void Shape::scale(float x, float y) {
    Transform matrix = Transform::Identity;
    matrix.scale(x, y);
    transform(matrix);
}

void Shape::scale(const Vec2& scale) {
    Transform matrix = Transform::Identity;
    matrix.scale(scale.x, scale.y);
    transform(matrix);
}

void Shape::rotate(float angle) {
    Transform matrix = Transform::Identity;
    matrix.rotate(angle);
    transform(matrix);
}

void Shape::rotate(float angle, const Vec2& center) {
    Transform matrix = Transform::Identity;
    matrix.rotate(angle, center.x, center.y);
    transform(matrix);
}

void Shape::rotate(float angle, float cx, float cy) {
    Transform matrix = Transform::Identity;
    matrix.rotate(angle, cx, cy);
    transform(matrix);
}

void Shape::transform(const Transform& matrix) {
    for (std::size_t i = 0; i < m_points.size(); ++i)
        m_points[i] = matrix.transform(m_points[i]);
    for (auto& hole : m_holes)
        hole.transform(matrix);
    m_stale = true;
}

void Shape::set_radius(std::size_t index, float radius, std::size_t smoothness) {
    if (radius >= 0.0f) {
        m_radii[index]      = radius;
        m_smoothness[index] = smoothness;
        m_stale             = true;
    }
}

float Shape::radius(std::size_t index) const { return m_radii[index]; }

void Shape::set_radii(float radius, std::size_t smoothness) {
    if (radius >= 0.0f) {
        for (std::size_t i = 0; i < point_count(); ++i) {
            m_radii[i]      = radius;
            m_smoothness[i] = smoothness;
        }
        m_stale = true;
    }
}

void Shape::set_radii(const std::vector<float>& radii) {
    assert(m_radii.size() == radii.size());
    m_radii = radii;
    m_stale = true;
}

const std::vector<float>& Shape::radii() const { return m_radii; }

std::size_t Shape::vertex_count() const {
    update_if_stale();
    return m_vertices.size();
}

const std::vector<Vec2>& Shape::vertices() const {
    update_if_stale();
    return m_vertices;
}

void Shape::apply_radii() {
    update_if_stale();
    set_points(m_vertices);
    set_radii(0.0f);
    m_stale = true;
}

void Shape::set_hole_count(std::size_t count) {
    m_holes.resize(count);
    m_stale = true;
}

std::size_t Shape::hole_count() const { return m_holes.size(); }

void Shape::set_hole(std::size_t index, const Shape& hole) {
    hole.update_if_stale();
    m_holes[index] = hole;
    m_stale        = true;
}

const Shape& Shape::hole(std::size_t index) const { return m_holes[index]; }

std::vector<Shape>& Shape::holes() { return m_holes; }

void Shape::push_back_hole(const Shape& hole) {
    hole.update_if_stale();
    m_holes.push_back(hole);
    m_stale = true;
}

Rect Shape::bounds(QueryMode mode) const {
    update_if_stale();
    if (mode == Points)
        return m_pointsBounds;
    else
        return m_verticesBounds;
}

bool Shape::contains(const Vec2& point, QueryMode mode) const {
    update_if_stale();
    // test holes
    for (auto& hole : m_holes) {
        if (hole.contains(point, mode))
            return false;
    }
    if (mode == Points)
        return inside_polygon(m_points, point);
    else
        return inside_polygon(m_vertices, point);
}

float Shape::area(QueryMode mode) const {
    update_if_stale();
    float area = 0.0f;
    if (mode == Points)
        area = polygon_area(m_points);
    else
        area = polygon_area(m_vertices);
    for (auto& hole : m_holes)
        area -= hole.area(mode);
    return area;
}

bool Shape::is_convex() const { return mahi::gui::is_convex(m_points); }

//==============================================================================
// PRIVATE FUNCTIONS
//==============================================================================

void Shape::update_if_stale() const {
    if (m_stale) {
        update_vertices();
        update_bounds();
    }
}

void Shape::update_vertices() const {
    // precompute vertice count
    std::size_t verticeCount = 0;
    for (std::size_t i = 0; i < m_points.size(); ++i) {
        if (m_smoothness[i] == 0 || m_smoothness[i] == 1 || m_radii[i] <= 0.0f) {
            verticeCount++;
        } else {
            verticeCount += m_smoothness[i];
        }
    }
    // resize vertices
    m_vertices.resize(verticeCount);
    std::size_t j = 0;
    for (std::size_t i = 0; i < m_points.size(); ++i) {
        // round point if neededd
        if (m_radii[i] > 0.0f && m_smoothness[i] > 1) {
            // determine A, B, C
            Vec2 B = m_points[i];
            Vec2 A, C;
            if (i == 0) {
                A = m_points[m_points.size() - 1];
                C = m_points[i + 1];
            } else if (i == m_points.size() - 1) {
                A = m_points[i - 1];
                C = m_points[0];
            } else {
                A = m_points[i - 1];
                C = m_points[i + 1];
            }
            // Find directional vectors of line segments
            Vec2 V1 = B - A;
            Vec2 V2 = B - C;
            // Check if corner radius is longer than vectors
            if (m_radii[i] >= magnitude(V1) || m_radii[i] >= magnitude(V2)) {
                m_vertices = std::vector<Vec2>();
                return;
            }
            // Find unit vectors
            Vec2 U1 = unit(V1);
            Vec2 U2 = unit(V2);
            // Find unit normal vectors
            Vec2 N1 = normal(U1);
            Vec2 N2 = normal(U2);
            // Check direction of normal vector
            if (dot(N1, -V2) < 0.0f)
                N1 = -N1;
            if (dot(N2, -V1) < 0.0f)
                N2 = -N2;
            // Find end-points of offset lines
            Vec2 O11 = A + N1 * m_radii[i];
            Vec2 O10 = B + N1 * m_radii[i];
            Vec2 O22 = C + N2 * m_radii[i];
            Vec2 O20 = B + N2 * m_radii[i];
            // Find intersection point of offset lines
            Vec2 I = intersection(O11, O10, O22, O20);
            // Find tangent points
            Vec2 T1 = I + -N1 * m_radii[i];
            Vec2 T2 = I + -N2 * m_radii[i];
            // TODO: check if tangent points are on line segments
            // ...
            // compute angles
            float              angle1 = std::atan2(T1.y - I.y, T1.x - I.x);
            float              angle2 = std::atan2(T2.y - I.y, T2.x - I.x);
            std::vector<float> angles(m_smoothness[i]);
            if (std::abs(angle1 - angle2) < util::PI)
                util::linspace(angle1, angle2, angles);
            else
                util::linspace(util::wrap_to_2pi(angle1), util::wrap_to_2pi(angle2), angles);
            // compute and set vertices
            for (std::size_t k = 0; k < angles.size(); k++) {
                m_vertices[j].x = m_radii[i] * std::cos(angles[k]) + I.x;
                m_vertices[j].y = m_radii[i] * std::sin(angles[k]) + I.y;
                j++;
            }
        }
        // otherwise set vertex equal to point
        else {
            m_vertices[j] = m_points[i];
            j++;
        }
    }
}

void Shape::update_bounds() const {
    // update points bounds
    if (m_points.size() > 0) {
        float left   = m_points[0].x;
        float top    = m_points[0].y;
        float right  = m_points[0].x;
        float bottom = m_points[0].y;
        for (std::size_t i = 1; i < m_points.size(); ++i) {
            // Update left and right
            if (m_points[i].x < left)
                left = m_points[i].x;
            else if (m_points[i].x > right)
                right = m_points[i].x;
            // Update top and bottom
            if (m_points[i].y < top)
                top = m_points[i].y;
            else if (m_points[i].y > bottom)
                bottom = m_points[i].y;
        }
        m_pointsBounds = Rect(left, top, right - left, bottom - top);
    } else {
        // Array is empty
        m_pointsBounds = Rect();
    }
    // update vertices bounds
    if (m_vertices.size() > 0) {
        float left   = m_vertices[0].x;
        float top    = m_vertices[0].y;
        float right  = m_vertices[0].x;
        float bottom = m_vertices[0].y;
        for (std::size_t i = 1; i < m_vertices.size(); ++i) {
            // Update left and right
            if (m_vertices[i].x < left)
                left = m_vertices[i].x;
            else if (m_vertices[i].x > right)
                right = m_vertices[i].x;
            // Update top and bottom
            if (m_vertices[i].y < top)
                top = m_vertices[i].y;
            else if (m_vertices[i].y > bottom)
                bottom = m_vertices[i].y;
        }
        m_verticesBounds = Rect(left, top, right - left, bottom - top);
    } else {
        // Array is empty
        m_verticesBounds = Rect();
    }
}
//==============================================================================
// PUBLIC STATIC FUNCTIONS
//==============================================================================

Shape offset_shape(const Shape& shape, float offset, OffsetType type) {
    ClipperLib::Path          subj = to_clipper(shape.vertices());
    ClipperLib::ClipperOffset co;
    switch (type) {
        case Miter: co.AddPath(subj, ClipperLib::jtMiter, ClipperLib::etClosedPolygon); break;
        case Round: co.AddPath(subj, ClipperLib::jtRound, ClipperLib::etClosedPolygon); break;
        case Square: co.AddPath(subj, ClipperLib::jtSquare, ClipperLib::etClosedPolygon);
    }
    ClipperLib::Paths solution;
    co.Execute(solution, offset * CLIPPER_PREC);
    Shape offset_shape;
    if (solution.size() > 0)
        offset_shape.set_points(from_clipper(solution[0]));
    for (std::size_t i = 0; i < shape.hole_count(); ++i) {
        co.Clear();
        ClipperLib::Path holeSubj = to_clipper(shape.hole(i).vertices());
        switch (type) {
            case Miter:
                co.AddPath(holeSubj, ClipperLib::jtMiter, ClipperLib::etClosedPolygon);
                break;
            case Round:
                co.AddPath(holeSubj, ClipperLib::jtRound, ClipperLib::etClosedPolygon);
                break;
            case Square: co.AddPath(holeSubj, ClipperLib::jtSquare, ClipperLib::etClosedPolygon);
        }
        solution.clear();
        co.Execute(solution, -offset * CLIPPER_PREC);
        if (solution.size() > 0) {
            Shape hole;
            hole.set_points(from_clipper(solution[0]));
            offset_shape.push_back_hole(hole);
        }
    }

    return offset_shape;
}

std::vector<Shape> clip_shapes(const Shape& subject, const Shape& clip, ClipType type) {
    ClipperLib::Paths subj;
    ClipperLib::Paths clp;

    subj << to_clipper(subject.vertices());
    clp << to_clipper(clip.vertices());

    for (std::size_t i = 0; i < subject.hole_count(); ++i)
        subj << to_clipper(subject.hole(i).vertices());
    for (std::size_t i = 0; i < clip.hole_count(); ++i)
        clp << to_clipper(clip.hole(i).vertices());

    ClipperLib::Clipper clpr;
    clpr.AddPaths(subj, ClipperLib::ptSubject, true);
    clpr.AddPaths(clp, ClipperLib::ptClip, true);

    ClipperLib::PolyTree polyTree;
    switch (type) {
        case Intersection:
            clpr.Execute(ClipperLib::ctIntersection, polyTree, ClipperLib::pftEvenOdd,
                         ClipperLib::pftEvenOdd);
            break;
        case Union:
            clpr.Execute(ClipperLib::ctUnion, polyTree, ClipperLib::pftEvenOdd,
                         ClipperLib::pftEvenOdd);
            break;
        case Difference:
            clpr.Execute(ClipperLib::ctDifference, polyTree, ClipperLib::pftEvenOdd,
                         ClipperLib::pftEvenOdd);
            break;
        case Exclusion:
            clpr.Execute(ClipperLib::ctXor, polyTree, ClipperLib::pftEvenOdd,
                         ClipperLib::pftEvenOdd);
            break;
    }
    std::vector<Shape> clippedShapes(polyTree.ChildCount());
    for (std::size_t i = 0; i < static_cast<std::size_t>(polyTree.ChildCount()); ++i) {
        clippedShapes[i].set_points(from_clipper(polyTree.Childs[i]->Contour));
        // add holes
        for (std::size_t j = 0; j < static_cast<std::size_t>(polyTree.Childs[i]->ChildCount());
             ++j) {
            if (polyTree.Childs[i]->Childs[j]->IsHole()) {
                Shape hole;
                hole.set_points(from_clipper(polyTree.Childs[i]->Childs[j]->Contour));
                clippedShapes[i].push_back_hole(hole);
            }
        }
    }
    return clippedShapes;
}

}  // namespace gui
}  // namespace mahi
