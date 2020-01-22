#pragma once

#include <mahi/Color.hpp>

namespace mahi::gui {
namespace Random {

/// Returns an int random number in the interval [min, max].
int range(int min, int max);

/// Returns an unsigned int random number in the interval [min, max].
unsigned int range(unsigned int min, unsigned int max);

/// Returns a float random number in the interval [min, max].
float range(float min, float max);

/// Returns a random Color
Color color();

/// Returns a random color between two colors in RGB space
Color color(const Color& color1, const Color& color2);

/// Sets the seed of the random number generator. Without calling this function, the seed is different at each program startup.
/// Setting the seed manually is useful when you want to reproduce a given sequence of random numbers. 
void setSeed(unsigned long seed);

} // namespace Random
} // namespace mahi::gui
