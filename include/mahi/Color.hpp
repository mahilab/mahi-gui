#pragma once

#include <string>

namespace mahi::gui {

/// Color Color POD type representation
struct Color {
    float r; /// red component   [0 to 1]
    float g; /// blue component  [0 to 1]
    float b; /// green component [0 to 1]
    float a; /// alpha component [0 to 1]
};

/// HSV Color POD type representation
struct HSV {
    float h; // angle in degrees [0 to 360]
    float s; // a fraction       [0 to 1]
    float v; // a fraction       [0 to 1]
    float a; // alpha component  [0 to 1]
};

/// Creates an RGB Color from an HSV
Color toRgb(const HSV& hsv);

/// Creates a RGB Color from a hex code
Color toRgb(std::string hex);

/// Creaets an HSV from an RGB Color
HSV toHsv(const Color& color);

/// Creates an HSV from a hex code
HSV toHsv(std::string hex);

/// Convenienve function to add alpha to a color
Color withAlpha(Color color, float a);

/// Returns the luminance of a Color
float luminance(const Color& color);

} // namespace mahi::gui
