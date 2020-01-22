#pragma once

#include <string>

namespace mahi::gui {

/// Color Color POD type representation
struct Color {
    float r = 1.0f; /// red component   [0 to 1]
    float g = 1.0f; /// blue component  [0 to 1]
    float b = 1.0f; /// green component [0 to 1]
    float a = 1.0f; /// alpha component [0 to 1]
};

/// HSV Color POD type representation
struct HSV {
    float h = 1.0f; // angle in degrees [0 to 1]
    float s = 1.0f; // a fraction       [0 to 1]
    float v = 1.0f; // a fraction       [0 to 1]
    float a = 1.0f; // alpha component  [0 to 1]
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


///////////////////////////////////////////////////////////////////////////////

constexpr Color fromBits(uint8_t r, uint8_t g, uint8_t b) { return {r / 255.0f, g / 255.0f, b / 255.0f, 1.0f}; }

namespace Pinks
{
    constexpr Color Pink = fromBits(255,192,203);
    constexpr Color LightPink = fromBits(255,182,193);
    constexpr Color HotPink = fromBits(255,105,180);
    constexpr Color DeepPink = fromBits(255,20,147);
    constexpr Color PaleVioletRed = fromBits(219,112,147);
    constexpr Color MediumVioletRed = fromBits(199,21,133);
}

namespace Reds
{
    constexpr Color LightSalmon = fromBits(255,160,122);
    constexpr Color Salmon = fromBits(250,128,114);
    constexpr Color DarkSalmon = fromBits(233,150,122);
    constexpr Color LightCoral = fromBits(240,128,128);
    constexpr Color IndianRed = fromBits(205,92,92);
    constexpr Color Crimson = fromBits(220,20,60);
    constexpr Color FireBrick = fromBits(178,34,34);
    constexpr Color DarkRed = fromBits(139,0,0);
    constexpr Color Red = fromBits(255,0,0);
}

namespace Oranges
{
    constexpr Color OrangeRed = fromBits(255,69,0);
    constexpr Color Tomato = fromBits(255,99,71);
    constexpr Color Coral = fromBits(255,127,80);
    constexpr Color DarkOrange = fromBits(255,140,0);
    constexpr Color Orange = fromBits(255,165,0);
}

namespace Yellows
{
    constexpr Color Yellow = fromBits(255,255,0);
    constexpr Color LightYellow = fromBits(255,255,224);
    constexpr Color LemonChiffon = fromBits(255,250,205);
    constexpr Color LightGoldenrodYellow  = fromBits(250,250,210);
    constexpr Color PapayaWhip = fromBits(255,239,213);
    constexpr Color Moccasin = fromBits(255,228,181);
    constexpr Color PeachPuff = fromBits(255,218,185);
    constexpr Color PaleGoldenrod = fromBits(238,232,170);
    constexpr Color Khaki = fromBits(240,230,140);
    constexpr Color DarkKhaki = fromBits(189,183,107);
    constexpr Color Gold = fromBits(255,215,0);
}

namespace Browns
{
    constexpr Color Cornsilk = fromBits(255,248,220);
    constexpr Color BlanchedAlmond = fromBits(255,235,205);
    constexpr Color Bisque = fromBits(255,228,196);
    constexpr Color NavajoWhite = fromBits(255,222,173);
    constexpr Color Wheat = fromBits(245,222,179);
    constexpr Color BurlyWood = fromBits(222,184,135);
    constexpr Color Tan = fromBits(210,180,140);
    constexpr Color RosyBrown = fromBits(188,143,143);
    constexpr Color SandyBrown = fromBits(244,164,96);
    constexpr Color Goldenrod = fromBits(218,165,32);
    constexpr Color DarkGoldenrod = fromBits(184,134,11);
    constexpr Color Peru = fromBits(205,133,63);
    constexpr Color Chocolate = fromBits(210,105,30);
    constexpr Color SaddleBrown = fromBits(139,69,19);
    constexpr Color Sienna = fromBits(160,82,45);
    constexpr Color Brown = fromBits(165,42,42);
    constexpr Color Maroon = fromBits(128,0,0);
}

namespace Greens
{
    constexpr Color DarkOliveGreen = fromBits(85,107,47);
    constexpr Color Olive = fromBits(128,128,0);
    constexpr Color OliveDrab = fromBits(107,142,35);
    constexpr Color YellowGreen = fromBits(154,205,50);
    constexpr Color LimeGreen = fromBits(50,205,50);
    constexpr Color Lime = fromBits(0,255,0);
    constexpr Color LawnGreen = fromBits(124,252,0);
    constexpr Color Chartreuse = fromBits(127,255,0);
    constexpr Color GreenYellow = fromBits(173,255,47);
    constexpr Color SpringGreen = fromBits(0,255,127);
    constexpr Color MediumSpringGreen  = fromBits(0,250,154);
    constexpr Color LightGreen = fromBits(144,238,144);
    constexpr Color PaleGreen = fromBits(152,251,152);
    constexpr Color DarkSeaGreen = fromBits(143,188,143);
    constexpr Color MediumAquamarine = fromBits(102,205,170);
    constexpr Color MediumSeaGreen = fromBits(60,179,113);
    constexpr Color SeaGreen = fromBits(46,139,87);
    constexpr Color ForestGreen = fromBits(34,139,34);
    constexpr Color Green = fromBits(0,128,0);
    constexpr Color DarkGreen = fromBits(0,100,0);
}

namespace Cyans
{
    constexpr Color Aqua = fromBits(0,255,255);
    constexpr Color Cyan = fromBits(0,255,255);
    constexpr Color LightCyan = fromBits(224,255,255);
    constexpr Color PaleTurquoise = fromBits(175,238,238);
    constexpr Color Aquamarine = fromBits(127,255,212);
    constexpr Color Turquoise = fromBits(64,224,208);
    constexpr Color MediumTurquoise = fromBits(72,209,204);
    constexpr Color DarkTurquoise = fromBits(0,206,209);
    constexpr Color LightSeaGreen = fromBits(32,178,170);
    constexpr Color CadetBlue = fromBits(95,158,160);
    constexpr Color DarkCyan = fromBits(0,139,139);
    constexpr Color Teal = fromBits(0,128,128);
}

namespace Blues
{
    constexpr Color LightSteelBlue = fromBits(176,196,222);
    constexpr Color PowderBlue = fromBits(176,224,230);
    constexpr Color LightBlue = fromBits(173,216,230);
    constexpr Color SkyBlue = fromBits(135,206,235);
    constexpr Color LightSkyBlue = fromBits(135,206,250);
    constexpr Color DeepSkyBlue = fromBits(0,191,255);
    constexpr Color DodgerBlue = fromBits(30,144,255);
    constexpr Color CornflowerBlue = fromBits(100,149,237);
    constexpr Color SteelBlue = fromBits(70,130,180);
    constexpr Color RoyalBlue = fromBits(65,105,225);
    constexpr Color Blue = fromBits(0,0,255);
    constexpr Color MediumBlue = fromBits(0,0,205);
    constexpr Color DarkBlue = fromBits(0,0,139);
    constexpr Color Navy = fromBits(0,0,128);
    constexpr Color MidnightBlue = fromBits(25,25,112);
}

namespace Purples
{
    constexpr Color Lavender = fromBits(230,230,250);
    constexpr Color Thistle = fromBits(216,191,216);
    constexpr Color Plum = fromBits(221,160,221);
    constexpr Color Violet = fromBits(238,130,238);
    constexpr Color Orchid = fromBits(218,112,214);
    constexpr Color Fuchsia = fromBits(255,0,255);
    constexpr Color Magenta = fromBits(255,0,255);
    constexpr Color MediumOrchid = fromBits(186,85,211);
    constexpr Color MediumPurple = fromBits(147,112,219);
    constexpr Color BlueViolet = fromBits(138,43,226);
    constexpr Color DarkViolet = fromBits(148,0,211);
    constexpr Color DarkOrchid = fromBits(153,50,204);
    constexpr Color DarkMagenta = fromBits(139,0,139);
    constexpr Color Purple = fromBits(128,0,128);
    constexpr Color Indigo = fromBits(75,0,130);
    constexpr Color DarkSlateBlue = fromBits(72,61,139);
    constexpr Color SlateBlue = fromBits(106,90,205);
    constexpr Color MediumSlateBlue  = fromBits(123,104,238);
}

namespace Whites
{
    constexpr Color White = {1,1,1,1};
    constexpr Color Snow = fromBits(255,250,250);
    constexpr Color Honeydew = fromBits(240,255,240);
    constexpr Color MintCream = fromBits(245,255,250);
    constexpr Color Azure = fromBits(240,255,255);
    constexpr Color AliceBlue = fromBits(240,248,255);
    constexpr Color GhostWhite = fromBits(248,248,255);
    constexpr Color WhiteSmoke = fromBits(245,245,245);
    constexpr Color Seashell = fromBits(255,245,238);
    constexpr Color Beige = fromBits(245,245,220);
    constexpr Color OldLace = fromBits(253,245,230);
    constexpr Color FloralWhite = fromBits(255,250,240);
    constexpr Color Ivory = fromBits(255,255,240);
    constexpr Color AntiqueWhite = fromBits(250,235,215);
    constexpr Color Linen = fromBits(250,240,230);
    constexpr Color LavenderBlush = fromBits(255,240,245);
    constexpr Color MistyRose = fromBits(255,228,225);
}

namespace Grays
{
    constexpr Color Gainsboro = fromBits(220,220,220);
    constexpr Color LightGray = fromBits(211,211,211);
    constexpr Color Silver = fromBits(192,192,192);
    constexpr Color DarkGray = fromBits(169,169,169);
    constexpr Color Gray = fromBits(128,128,128);
    constexpr Color DimGray = fromBits(105,105,105);
    constexpr Color LightSlateGray = fromBits(119,136,153);
    constexpr Color SlateGray = fromBits(112,128,144);
    constexpr Color DarkSlateGray = fromBits(47,79,79);
    constexpr Color Black  = {0,0,0,0};
    constexpr Color Gray5  = {0.05f,0.05f,0.05f,1.0f};
    constexpr Color Gray10 = {0.1f,0.1f,0.1f,1.0f};
    constexpr Color Gray20 = {0.2f,0.2f,0.2f,1.0f};
    constexpr Color Gray30 = {0.3f,0.3f,0.3f,1.0f};
    constexpr Color Gray40 = {0.4f,0.4f,0.4f,1.0f};
    constexpr Color Gray50 = {0.5f,0.5f,0.5f,1.0f};
    constexpr Color Gray60 = {0.6f,0.6f,0.6f,1.0f};
    constexpr Color Gray70 = {0.7f,0.7f,0.7f,1.0f};
    constexpr Color Gray80 = {0.8f,0.8f,0.8f,1.0f};
    constexpr Color Gray90 = {0.9f,0.9f,0.9f,1.0f};
    constexpr Color Gray95 = {0.95f,0.95f,0.95f,1.0f};
}

} // namespace mahi::gui
