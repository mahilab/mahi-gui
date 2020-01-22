#include <mahi/Color.hpp>
// #include <mahi/Math.hpp>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <imgui.h>

namespace mahi::gui {

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

} // namespace mahi::gui
