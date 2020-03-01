#include <mahi/gui/Print.hpp>
#include <mahi/gui/Math.hpp>
#include <sstream>

namespace mahi::gui {

std::string numToStr(float number) {
    int order = Math::orderOfMagnitude(number);
    std::size_t prec = Math::precision(order);
    std::stringstream stream;
    stream << std::fixed << std::setprecision(prec) << number;
    return stream.str();
}

std::string numToStr(double number) {
    int order = Math::orderOfMagnitude(static_cast<float>(number));
    std::size_t prec = Math::precision(order);
    std::stringstream stream;
    stream << std::fixed << std::setprecision(prec) << number;
    return stream.str();
}

std::ostream & operator << (std::ostream &out, const Color& rgb) {
    out << "(R:" << rgb.r << ",G:" << rgb.g << ",B:" << rgb.b << ",A:" << rgb.a << ")";
    return out;
}

std::ostream & operator << (std::ostream &out, const HSV& hsv) {
    out << "(H:" << hsv.h << ",S:" << hsv.s << ",V:" << hsv.v  << ",A:" << hsv.a << ")";
    return out;
}

} // namespace mahi::gui
