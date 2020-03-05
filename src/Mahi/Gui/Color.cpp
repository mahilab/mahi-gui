#include <Mahi/Gui/Color.hpp>
#include <Mahi/Util/Random.hpp>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <imgui.h>

namespace mahi {
namespace gui {

Color toRgb(const HSV& in) {
    Color out;
    ImGui::ColorConvertHSVtoRGB(in.h, in.s, in.v, out.r, out.g, out.b);
    return out;
}

Color toRgb(std::string hex) {
    if (hex[0] == '#')
        hex.erase(0,1);
    unsigned int r, g, b, a;
    if (hex.length() == 6) {
        #ifdef _WIN32
            sscanf_s(hex.c_str(), "%02x%02x%02x", &r, &g, &b);
        #else
            sscanf(hex.c_str(), "%02x%02x%02x", &r, &g, &b);
        #endif
        return Color{static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f,  1.0f};
    }
    else if (hex.length() == 8) {
        #ifdef _WIN32
            sscanf_s(hex.c_str(), "%02x%02x%02x%02x", &r, &g, &b, &a);
        #else
            sscanf(hex.c_str(), "%02x%02x%02x%02x",  &r, &g, &b, &a);
        #endif
        return Color{static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, static_cast<float>(a) / 255.0f};
    }
    else
        return Color{1.0f,1.0f,1.0f,1.0f};
}

HSV toHsv(const Color& in) {
    HSV out;
    ImGui::ColorConvertRGBtoHSV(in.r, in.g, in.b, out.h, out.s, out.v);
    return out;
}

HSV toHsv(std::string hex) {
    return toHsv(toRgb(hex));
}

Color withAlpha(Color color, float a) {
    color.a = a;
    return color;
}

float luminance(const Color& color) {
    return ( 0.299f * color.r + 0.587f * color.g + 0.114f * color.b);
}

Color random_color() {
    Color color;
    color.r = util::random_range(0.0f,1.0f);
    color.g = util::random_range(0.0f,1.0f);
    color.b = util::random_range(0.0f,1.0f);
    return color;
}

Color random_color(const Color& color1, const Color& color2) {
    Color color;
    color.r = util::random_range(color1.r, color2.r);
    color.g = util::random_range(color1.g, color2.g);
    color.b = util::random_range(color1.b, color2.b);
    color.a = util::random_range(color1.a, color2.a);
    return color;
}

std::ostream & operator << (std::ostream &out, const Color& rgb) {
    out << "(R:" << rgb.r << ",G:" << rgb.g << ",B:" << rgb.b << ",A:" << rgb.a << ")";
    return out;
}

std::ostream & operator << (std::ostream &out, const HSV& hsv) {
    out << "(H:" << hsv.h << ",S:" << hsv.s << ",V:" << hsv.v  << ",A:" << hsv.a << ")";
    return out;
}

} // namespace gui
} // namespace mahi

