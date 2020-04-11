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

#include <string>
#include <nanovg.h>

namespace mahi {
namespace gui {

/// Alias for NVGColor, a float RGBA representation
typedef NVGcolor Color;

/// HSV Color POD type representation
struct HSV {
    float h = 1.0f;  // angle in degrees [0 to 1]
    float s = 1.0f;  // a fraction       [0 to 1]
    float v = 1.0f;  // a fraction       [0 to 1]
    float a = 1.0f;  // alpha component  [0 to 1]
};

/// Creates an RGB Color from an HSV
Color to_rgb(const HSV& hsv);
/// Creates a RGB Color from a hex code
Color to_rgb(std::string hex);
/// Creaets an HSV from an RGB Color
HSV to_hsv(const Color& color);
/// Creates an HSV from a hex code
HSV to_hsv(std::string hex);
/// Convenienve function to add alpha to a color
Color with_alpha(Color color, float a);
/// Returns the luminance of a Color
float luminance(const Color& color);
/// Returns a random Color
Color random_color();
/// Returns a random color between two colors in RGB space
Color random_color(const Color& color1, const Color& color2);
/// Steam a color
std::ostream& operator<<(std::ostream& out, const Color& color);
/// Stream an HSV
std::ostream& operator<<(std::ostream& out, const HSV& hsv);

///////////////////////////////////////////////////////////////////////////////

/// Makes a color from RGBA values in range 0 to 255
constexpr Color RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return {r / 255.0f, g / 255.0f, b / 255.0f, 1.0f};
}

/// Most Commonly Used Colors
namespace Colors {
constexpr Color Auto        = {0, 0, 0, -1};
constexpr Color Transparent = {0, 0, 0, 0};
constexpr Color Black       = {0, 0, 0, 1};
constexpr Color Gray        = {0.5f, 0.5f, 0.5f, 1.0f};
constexpr Color White       = {1, 1, 1, 1};
constexpr Color Red         = {1, 0, 0, 1};
constexpr Color Green       = {0, 1, 0, 1};
constexpr Color Blue        = {0, 0, 1, 0};
constexpr Color Cyan        = {0, 1, 1, 1};
constexpr Color Magenta     = {1, 0, 1, 1};
constexpr Color Yellow      = {1, 1, 0, 1};
constexpr Color Orange      = RGBA(255, 165, 0);
constexpr Color Pink        = RGBA(255, 192, 203);
constexpr Color Purple      = RGBA(128, 0, 128);
constexpr Color Violet      = RGBA(238, 130, 238);
constexpr Color Gold        = RGBA(255, 215, 0);
constexpr Color Brown       = RGBA(165, 42, 42);
}  // namespace Colors

/// Web Colors Pinks
namespace Pinks {
constexpr Color Pink            = RGBA(255, 192, 203);
constexpr Color LightPink       = RGBA(255, 182, 193);
constexpr Color HotPink         = RGBA(255, 105, 180);
constexpr Color DeepPink        = RGBA(255, 20, 147);
constexpr Color PaleVioletRed   = RGBA(219, 112, 147);
constexpr Color MediumVioletRed = RGBA(199, 21, 133);
}  // namespace Pinks

/// Web Colors Reds
namespace Reds {
constexpr Color LightSalmon = RGBA(255, 160, 122);
constexpr Color Salmon      = RGBA(250, 128, 114);
constexpr Color DarkSalmon  = RGBA(233, 150, 122);
constexpr Color LightCoral  = RGBA(240, 128, 128);
constexpr Color IndianRed   = RGBA(205, 92, 92);
constexpr Color Crimson     = RGBA(220, 20, 60);
constexpr Color FireBrick   = RGBA(178, 34, 34);
constexpr Color DarkRed     = RGBA(139, 0, 0);
constexpr Color Red         = RGBA(255, 0, 0);
}  // namespace Reds

/// Web Colors Oranges
namespace Oranges {
constexpr Color OrangeRed  = RGBA(255, 69, 0);
constexpr Color Tomato     = RGBA(255, 99, 71);
constexpr Color Coral      = RGBA(255, 127, 80);
constexpr Color DarkOrange = RGBA(255, 140, 0);
constexpr Color Orange     = RGBA(255, 165, 0);
}  // namespace Oranges

/// Web Colors Yellows
namespace Yellows {
constexpr Color Yellow               = RGBA(255, 255, 0);
constexpr Color LightYellow          = RGBA(255, 255, 224);
constexpr Color LemonChiffon         = RGBA(255, 250, 205);
constexpr Color LightGoldenrodYellow = RGBA(250, 250, 210);
constexpr Color PapayaWhip           = RGBA(255, 239, 213);
constexpr Color Moccasin             = RGBA(255, 228, 181);
constexpr Color PeachPuff            = RGBA(255, 218, 185);
constexpr Color PaleGoldenrod        = RGBA(238, 232, 170);
constexpr Color Khaki                = RGBA(240, 230, 140);
constexpr Color DarkKhaki            = RGBA(189, 183, 107);
constexpr Color Gold                 = RGBA(255, 215, 0);
}  // namespace Yellows

/// Web Colors Browns
namespace Browns {
constexpr Color Cornsilk       = RGBA(255, 248, 220);
constexpr Color BlanchedAlmond = RGBA(255, 235, 205);
constexpr Color Bisque         = RGBA(255, 228, 196);
constexpr Color NavajoWhite    = RGBA(255, 222, 173);
constexpr Color Wheat          = RGBA(245, 222, 179);
constexpr Color BurlyWood      = RGBA(222, 184, 135);
constexpr Color Tan            = RGBA(210, 180, 140);
constexpr Color RosyBrown      = RGBA(188, 143, 143);
constexpr Color SandyBrown     = RGBA(244, 164, 96);
constexpr Color Goldenrod      = RGBA(218, 165, 32);
constexpr Color DarkGoldenrod  = RGBA(184, 134, 11);
constexpr Color Peru           = RGBA(205, 133, 63);
constexpr Color Chocolate      = RGBA(210, 105, 30);
constexpr Color SaddleBrown    = RGBA(139, 69, 19);
constexpr Color Sienna         = RGBA(160, 82, 45);
constexpr Color Brown          = RGBA(165, 42, 42);
constexpr Color Maroon         = RGBA(128, 0, 0);
}  // namespace Browns

/// Web Colors Greens
namespace Greens {
constexpr Color DarkOliveGreen    = RGBA(85, 107, 47);
constexpr Color Olive             = RGBA(128, 128, 0);
constexpr Color OliveDrab         = RGBA(107, 142, 35);
constexpr Color YellowGreen       = RGBA(154, 205, 50);
constexpr Color LimeGreen         = RGBA(50, 205, 50);
constexpr Color Lime              = RGBA(0, 255, 0);
constexpr Color LawnGreen         = RGBA(124, 252, 0);
constexpr Color Chartreuse        = RGBA(127, 255, 0);
constexpr Color GreenYellow       = RGBA(173, 255, 47);
constexpr Color SpringGreen       = RGBA(0, 255, 127);
constexpr Color MediumSpringGreen = RGBA(0, 250, 154);
constexpr Color LightGreen        = RGBA(144, 238, 144);
constexpr Color PaleGreen         = RGBA(152, 251, 152);
constexpr Color DarkSeaGreen      = RGBA(143, 188, 143);
constexpr Color MediumAquamarine  = RGBA(102, 205, 170);
constexpr Color MediumSeaGreen    = RGBA(60, 179, 113);
constexpr Color SeaGreen          = RGBA(46, 139, 87);
constexpr Color ForestGreen       = RGBA(34, 139, 34);
constexpr Color Green             = RGBA(0, 128, 0);
constexpr Color DarkGreen         = RGBA(0, 100, 0);
}  // namespace Greens

/// Web Colors Cyans
namespace Cyans {
constexpr Color Aqua            = RGBA(0, 255, 255);
constexpr Color Cyan            = RGBA(0, 255, 255);
constexpr Color LightCyan       = RGBA(224, 255, 255);
constexpr Color PaleTurquoise   = RGBA(175, 238, 238);
constexpr Color Aquamarine      = RGBA(127, 255, 212);
constexpr Color Turquoise       = RGBA(64, 224, 208);
constexpr Color MediumTurquoise = RGBA(72, 209, 204);
constexpr Color DarkTurquoise   = RGBA(0, 206, 209);
constexpr Color LightSeaGreen   = RGBA(32, 178, 170);
constexpr Color CadetBlue       = RGBA(95, 158, 160);
constexpr Color DarkCyan        = RGBA(0, 139, 139);
constexpr Color Teal            = RGBA(0, 128, 128);
}  // namespace Cyans

/// Web Colors Blues
namespace Blues {
constexpr Color LightSteelBlue = RGBA(176, 196, 222);
constexpr Color PowderBlue     = RGBA(176, 224, 230);
constexpr Color LightBlue      = RGBA(173, 216, 230);
constexpr Color SkyBlue        = RGBA(135, 206, 235);
constexpr Color LightSkyBlue   = RGBA(135, 206, 250);
constexpr Color DeepSkyBlue    = RGBA(0, 191, 255);
constexpr Color DodgerBlue     = RGBA(30, 144, 255);
constexpr Color CornflowerBlue = RGBA(100, 149, 237);
constexpr Color SteelBlue      = RGBA(70, 130, 180);
constexpr Color RoyalBlue      = RGBA(65, 105, 225);
constexpr Color Blue           = RGBA(0, 0, 255);
constexpr Color MediumBlue     = RGBA(0, 0, 205);
constexpr Color DarkBlue       = RGBA(0, 0, 139);
constexpr Color Navy           = RGBA(0, 0, 128);
constexpr Color MidnightBlue   = RGBA(25, 25, 112);
}  // namespace Blues

/// Web Colors Purples
namespace Purples {
constexpr Color Lavender        = RGBA(230, 230, 250);
constexpr Color Thistle         = RGBA(216, 191, 216);
constexpr Color Plum            = RGBA(221, 160, 221);
constexpr Color Violet          = RGBA(238, 130, 238);
constexpr Color Orchid          = RGBA(218, 112, 214);
constexpr Color Fuchsia         = RGBA(255, 0, 255);
constexpr Color Magenta         = RGBA(255, 0, 255);
constexpr Color MediumOrchid    = RGBA(186, 85, 211);
constexpr Color MediumPurple    = RGBA(147, 112, 219);
constexpr Color BlueViolet      = RGBA(138, 43, 226);
constexpr Color DarkViolet      = RGBA(148, 0, 211);
constexpr Color DarkOrchid      = RGBA(153, 50, 204);
constexpr Color DarkMagenta     = RGBA(139, 0, 139);
constexpr Color Purple          = RGBA(128, 0, 128);
constexpr Color Indigo          = RGBA(75, 0, 130);
constexpr Color DarkSlateBlue   = RGBA(72, 61, 139);
constexpr Color SlateBlue       = RGBA(106, 90, 205);
constexpr Color MediumSlateBlue = RGBA(123, 104, 238);
}  // namespace Purples

/// Web Colors Whites
namespace Whites {
constexpr Color White         = {1, 1, 1, 1};
constexpr Color Snow          = RGBA(255, 250, 250);
constexpr Color Honeydew      = RGBA(240, 255, 240);
constexpr Color MintCream     = RGBA(245, 255, 250);
constexpr Color Azure         = RGBA(240, 255, 255);
constexpr Color AliceBlue     = RGBA(240, 248, 255);
constexpr Color GhostWhite    = RGBA(248, 248, 255);
constexpr Color WhiteSmoke    = RGBA(245, 245, 245);
constexpr Color Seashell      = RGBA(255, 245, 238);
constexpr Color Beige         = RGBA(245, 245, 220);
constexpr Color OldLace       = RGBA(253, 245, 230);
constexpr Color FloralWhite   = RGBA(255, 250, 240);
constexpr Color Ivory         = RGBA(255, 255, 240);
constexpr Color AntiqueWhite  = RGBA(250, 235, 215);
constexpr Color Linen         = RGBA(250, 240, 230);
constexpr Color LavenderBlush = RGBA(255, 240, 245);
constexpr Color MistyRose     = RGBA(255, 228, 225);
}  // namespace Whites

/// Web Colors Grays and Percent Grays
namespace Grays {
constexpr Color Gainsboro      = RGBA(220, 220, 220);
constexpr Color LightGray      = RGBA(211, 211, 211);
constexpr Color Silver         = RGBA(192, 192, 192);
constexpr Color DarkGray       = RGBA(169, 169, 169);
constexpr Color Gray           = RGBA(128, 128, 128);
constexpr Color DimGray        = RGBA(105, 105, 105);
constexpr Color LightSlateGray = RGBA(119, 136, 153);
constexpr Color SlateGray      = RGBA(112, 128, 144);
constexpr Color DarkSlateGray  = RGBA(47, 79, 79);
constexpr Color Black          = {0, 0, 0, 1};
constexpr Color Gray5          = {0.05f, 0.05f, 0.05f, 1.0f};
constexpr Color Gray10         = {0.1f, 0.1f, 0.1f, 1.0f};
constexpr Color Gray20         = {0.2f, 0.2f, 0.2f, 1.0f};
constexpr Color Gray30         = {0.3f, 0.3f, 0.3f, 1.0f};
constexpr Color Gray40         = {0.4f, 0.4f, 0.4f, 1.0f};
constexpr Color Gray50         = {0.5f, 0.5f, 0.5f, 1.0f};
constexpr Color Gray60         = {0.6f, 0.6f, 0.6f, 1.0f};
constexpr Color Gray70         = {0.7f, 0.7f, 0.7f, 1.0f};
constexpr Color Gray80         = {0.8f, 0.8f, 0.8f, 1.0f};
constexpr Color Gray90         = {0.9f, 0.9f, 0.9f, 1.0f};
constexpr Color Gray95         = {0.95f, 0.95f, 0.95f, 1.0f};
}  // namespace Grays

}  // namespace gui
}  // namespace mahi