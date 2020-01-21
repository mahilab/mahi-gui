#pragma once

#include <Utility/Types.hpp>
#include <Graphics/Color.hpp>
#include <iostream>
#include <sstream>
#include <utility>
#include <vector>
#include <array>

namespace mahui {

//=============================================================================
// Helpers
//=============================================================================

std::string numToStr(float number);
std::string numToStr(double number);

//=============================================================================
// Stream Overloads
//=============================================================================

std::ostream & operator << (std::ostream &out, const Color& color);
std::ostream & operator << (std::ostream &out, const RGB& rgb);
std::ostream & operator << (std::ostream &out, const HSV& hsv);
std::ostream & operator << (std::ostream &out, const sf::String& str);

template <typename T>
std::ostream & operator << (std::ostream &out, const sf::Vector2<T>& value) {
    out << "(X:" << value.x << ",Y:" << value.y << ")";
    return out;
}

template <typename T>
std::ostream & operator << (std::ostream &out, const sf::Rect<T>& value) {
    out << "(L:" << value.left << ",T:" << value.top << ",W:" << value.width << ",H:" << value.height << ")";
    return out;
}

/// Streams an std::vector
template <class Streamable, typename T>
Streamable& operator<<(Streamable& s, const std::vector<T>& c) {
    if (c.size() == 0)
        return s;
    for (std::size_t i = 0; i < c.size() - 1; ++i) {
        s << c[i];
        s << " ";
    }
    s << c[c.size() - 1];
    return s;
}

/// Streams an std::array
template <class Streamable, typename T, std::size_t N>
Streamable& operator<<(Streamable& s, const std::array<T, N>& c) {
    if (c.size() == 0)
        return s;
    for (std::size_t i = 0; i < N - 1; ++i) {
        s << c[i];
        s << " ";
    }
    s << c[N - 1] ;
    return s;
}

//=============================================================================
// Print
//=============================================================================

template <typename T>
std::string str(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

template <typename Arg, typename... Args>
std::string str(Arg&& arg, Args&&... args) {
    std::stringstream ss;
    ss << std::forward<Arg>(arg);
    using expander = int[];
    (void)expander{0, (void(ss << ' ' << std::forward<Args>(args)), 0)...};
    return ss.str();
}

/// Prints a value to std::cout and then starts a new line
template <typename T>
inline void print(const T& value) {
    std::cout << value << std::endl;
}

/// Prints variadic number of values to std::cout with separating spaces and then starts a new line
template <typename Arg, typename... Args>
void print(Arg&& arg, Args&&... args) {
    std::cout << std::forward<Arg>(arg);
    using expander = int[];
    (void)expander{0, (void(std::cout << ' ' << std::forward<Args>(args)), 0)...};
    std::cout << std::endl;
}

} // namespace mahui
