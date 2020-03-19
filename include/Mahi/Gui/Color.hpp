#pragma once

#include <string>

namespace mahi {
namespace gui {

/// Color Color POD type representation
struct Color {
    float r = 1.0f;  /// red component   [0 to 1]
    float g = 1.0f;  /// blue component  [0 to 1]
    float b = 1.0f;  /// green component [0 to 1]
    float a = 1.0f;  /// alpha component [0 to 1]
};

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

/// Makes a color from RGB values in range 0 to 255
constexpr Color from_bits(uint8_t r, uint8_t g, uint8_t b) {
    return {r / 255.0f, g / 255.0f, b / 255.0f, 1.0f};
}

namespace Colors {
constexpr Color Black       = {0, 0, 0, 1};
constexpr Color White       = {1, 1, 1, 1};
constexpr Color Transparent = {0, 0, 0, 0};
constexpr Color Auto = {0, 0, 0, -1};  // any color with -1 alpha will be treated as Colors::Auto
}  // namespace Colors

namespace Pinks {
constexpr Color Pink            = from_bits(255, 192, 203);
constexpr Color LightPink       = from_bits(255, 182, 193);
constexpr Color HotPink         = from_bits(255, 105, 180);
constexpr Color DeepPink        = from_bits(255, 20, 147);
constexpr Color PaleVioletRed   = from_bits(219, 112, 147);
constexpr Color MediumVioletRed = from_bits(199, 21, 133);
}  // namespace Pinks

namespace Reds {
constexpr Color LightSalmon = from_bits(255, 160, 122);
constexpr Color Salmon      = from_bits(250, 128, 114);
constexpr Color DarkSalmon  = from_bits(233, 150, 122);
constexpr Color LightCoral  = from_bits(240, 128, 128);
constexpr Color IndianRed   = from_bits(205, 92, 92);
constexpr Color Crimson     = from_bits(220, 20, 60);
constexpr Color FireBrick   = from_bits(178, 34, 34);
constexpr Color DarkRed     = from_bits(139, 0, 0);
constexpr Color Red         = from_bits(255, 0, 0);
}  // namespace Reds

namespace Oranges {
constexpr Color OrangeRed  = from_bits(255, 69, 0);
constexpr Color Tomato     = from_bits(255, 99, 71);
constexpr Color Coral      = from_bits(255, 127, 80);
constexpr Color DarkOrange = from_bits(255, 140, 0);
constexpr Color Orange     = from_bits(255, 165, 0);
}  // namespace Oranges

namespace Yellows {
constexpr Color Yellow               = from_bits(255, 255, 0);
constexpr Color LightYellow          = from_bits(255, 255, 224);
constexpr Color LemonChiffon         = from_bits(255, 250, 205);
constexpr Color LightGoldenrodYellow = from_bits(250, 250, 210);
constexpr Color PapayaWhip           = from_bits(255, 239, 213);
constexpr Color Moccasin             = from_bits(255, 228, 181);
constexpr Color PeachPuff            = from_bits(255, 218, 185);
constexpr Color PaleGoldenrod        = from_bits(238, 232, 170);
constexpr Color Khaki                = from_bits(240, 230, 140);
constexpr Color DarkKhaki            = from_bits(189, 183, 107);
constexpr Color Gold                 = from_bits(255, 215, 0);
}  // namespace Yellows

namespace Browns {
constexpr Color Cornsilk       = from_bits(255, 248, 220);
constexpr Color BlanchedAlmond = from_bits(255, 235, 205);
constexpr Color Bisque         = from_bits(255, 228, 196);
constexpr Color NavajoWhite    = from_bits(255, 222, 173);
constexpr Color Wheat          = from_bits(245, 222, 179);
constexpr Color BurlyWood      = from_bits(222, 184, 135);
constexpr Color Tan            = from_bits(210, 180, 140);
constexpr Color RosyBrown      = from_bits(188, 143, 143);
constexpr Color SandyBrown     = from_bits(244, 164, 96);
constexpr Color Goldenrod      = from_bits(218, 165, 32);
constexpr Color DarkGoldenrod  = from_bits(184, 134, 11);
constexpr Color Peru           = from_bits(205, 133, 63);
constexpr Color Chocolate      = from_bits(210, 105, 30);
constexpr Color SaddleBrown    = from_bits(139, 69, 19);
constexpr Color Sienna         = from_bits(160, 82, 45);
constexpr Color Brown          = from_bits(165, 42, 42);
constexpr Color Maroon         = from_bits(128, 0, 0);
}  // namespace Browns

namespace Greens {
constexpr Color DarkOliveGreen    = from_bits(85, 107, 47);
constexpr Color Olive             = from_bits(128, 128, 0);
constexpr Color OliveDrab         = from_bits(107, 142, 35);
constexpr Color YellowGreen       = from_bits(154, 205, 50);
constexpr Color LimeGreen         = from_bits(50, 205, 50);
constexpr Color Lime              = from_bits(0, 255, 0);
constexpr Color LawnGreen         = from_bits(124, 252, 0);
constexpr Color Chartreuse        = from_bits(127, 255, 0);
constexpr Color GreenYellow       = from_bits(173, 255, 47);
constexpr Color SpringGreen       = from_bits(0, 255, 127);
constexpr Color MediumSpringGreen = from_bits(0, 250, 154);
constexpr Color LightGreen        = from_bits(144, 238, 144);
constexpr Color PaleGreen         = from_bits(152, 251, 152);
constexpr Color DarkSeaGreen      = from_bits(143, 188, 143);
constexpr Color MediumAquamarine  = from_bits(102, 205, 170);
constexpr Color MediumSeaGreen    = from_bits(60, 179, 113);
constexpr Color SeaGreen          = from_bits(46, 139, 87);
constexpr Color ForestGreen       = from_bits(34, 139, 34);
constexpr Color Green             = from_bits(0, 128, 0);
constexpr Color DarkGreen         = from_bits(0, 100, 0);
}  // namespace Greens

namespace Cyans {
constexpr Color Aqua            = from_bits(0, 255, 255);
constexpr Color Cyan            = from_bits(0, 255, 255);
constexpr Color LightCyan       = from_bits(224, 255, 255);
constexpr Color PaleTurquoise   = from_bits(175, 238, 238);
constexpr Color Aquamarine      = from_bits(127, 255, 212);
constexpr Color Turquoise       = from_bits(64, 224, 208);
constexpr Color MediumTurquoise = from_bits(72, 209, 204);
constexpr Color DarkTurquoise   = from_bits(0, 206, 209);
constexpr Color LightSeaGreen   = from_bits(32, 178, 170);
constexpr Color CadetBlue       = from_bits(95, 158, 160);
constexpr Color DarkCyan        = from_bits(0, 139, 139);
constexpr Color Teal            = from_bits(0, 128, 128);
}  // namespace Cyans

namespace Blues {
constexpr Color LightSteelBlue = from_bits(176, 196, 222);
constexpr Color PowderBlue     = from_bits(176, 224, 230);
constexpr Color LightBlue      = from_bits(173, 216, 230);
constexpr Color SkyBlue        = from_bits(135, 206, 235);
constexpr Color LightSkyBlue   = from_bits(135, 206, 250);
constexpr Color DeepSkyBlue    = from_bits(0, 191, 255);
constexpr Color DodgerBlue     = from_bits(30, 144, 255);
constexpr Color CornflowerBlue = from_bits(100, 149, 237);
constexpr Color SteelBlue      = from_bits(70, 130, 180);
constexpr Color RoyalBlue      = from_bits(65, 105, 225);
constexpr Color Blue           = from_bits(0, 0, 255);
constexpr Color MediumBlue     = from_bits(0, 0, 205);
constexpr Color DarkBlue       = from_bits(0, 0, 139);
constexpr Color Navy           = from_bits(0, 0, 128);
constexpr Color MidnightBlue   = from_bits(25, 25, 112);
}  // namespace Blues

namespace Purples {
constexpr Color Lavender        = from_bits(230, 230, 250);
constexpr Color Thistle         = from_bits(216, 191, 216);
constexpr Color Plum            = from_bits(221, 160, 221);
constexpr Color Violet          = from_bits(238, 130, 238);
constexpr Color Orchid          = from_bits(218, 112, 214);
constexpr Color Fuchsia         = from_bits(255, 0, 255);
constexpr Color Magenta         = from_bits(255, 0, 255);
constexpr Color MediumOrchid    = from_bits(186, 85, 211);
constexpr Color MediumPurple    = from_bits(147, 112, 219);
constexpr Color BlueViolet      = from_bits(138, 43, 226);
constexpr Color DarkViolet      = from_bits(148, 0, 211);
constexpr Color DarkOrchid      = from_bits(153, 50, 204);
constexpr Color DarkMagenta     = from_bits(139, 0, 139);
constexpr Color Purple          = from_bits(128, 0, 128);
constexpr Color Indigo          = from_bits(75, 0, 130);
constexpr Color DarkSlateBlue   = from_bits(72, 61, 139);
constexpr Color SlateBlue       = from_bits(106, 90, 205);
constexpr Color MediumSlateBlue = from_bits(123, 104, 238);
}  // namespace Purples

namespace Whites {
constexpr Color White         = {1, 1, 1, 1};
constexpr Color Snow          = from_bits(255, 250, 250);
constexpr Color Honeydew      = from_bits(240, 255, 240);
constexpr Color MintCream     = from_bits(245, 255, 250);
constexpr Color Azure         = from_bits(240, 255, 255);
constexpr Color AliceBlue     = from_bits(240, 248, 255);
constexpr Color GhostWhite    = from_bits(248, 248, 255);
constexpr Color WhiteSmoke    = from_bits(245, 245, 245);
constexpr Color Seashell      = from_bits(255, 245, 238);
constexpr Color Beige         = from_bits(245, 245, 220);
constexpr Color OldLace       = from_bits(253, 245, 230);
constexpr Color FloralWhite   = from_bits(255, 250, 240);
constexpr Color Ivory         = from_bits(255, 255, 240);
constexpr Color AntiqueWhite  = from_bits(250, 235, 215);
constexpr Color Linen         = from_bits(250, 240, 230);
constexpr Color LavenderBlush = from_bits(255, 240, 245);
constexpr Color MistyRose     = from_bits(255, 228, 225);
}  // namespace Whites

namespace Grays {
constexpr Color Gainsboro      = from_bits(220, 220, 220);
constexpr Color LightGray      = from_bits(211, 211, 211);
constexpr Color Silver         = from_bits(192, 192, 192);
constexpr Color DarkGray       = from_bits(169, 169, 169);
constexpr Color Gray           = from_bits(128, 128, 128);
constexpr Color DimGray        = from_bits(105, 105, 105);
constexpr Color LightSlateGray = from_bits(119, 136, 153);
constexpr Color SlateGray      = from_bits(112, 128, 144);
constexpr Color DarkSlateGray  = from_bits(47, 79, 79);
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