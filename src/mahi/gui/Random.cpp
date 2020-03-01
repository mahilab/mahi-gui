#include <mahi/gui/Random.hpp>
#include <cassert>
#include <ctime>
#include <random>

namespace mahi::gui {
namespace {

// Random generator engine (Multiply With Carry)
// Many thanks to Volkard Henkel for the implementation.
class RandomEngine {
public:
    // Type definition for usage inside Std.Random
    typedef unsigned int result_type;

public:
    // Constructor
    explicit RandomEngine(unsigned int seedVal) { seed(seedVal); }

    // Return random number
    unsigned int operator()() {
        const std::size_t a = 1967773755;

        x = a * (x & 0xffffffff) + (x >> 32);
        return static_cast<unsigned int>(x);
    }

    // set seed (compliant to Std.Random)
    void seed(unsigned int seedVal = 0) { x = seedVal + !seedVal; }

    // Return minimal value (compliant to Std.Random)
    static constexpr unsigned int min() { return 0; }

    // Return maximal value (compliant to Std.Random)
    static constexpr unsigned int max() { return 0xffffffff; }

private:
    std::size_t x;
};

// Function initializing the engine and its seed at startup time
RandomEngine createInitialEngine() {
    return RandomEngine(static_cast<unsigned long>(std::time(nullptr)));
}

// Pseudo random number generator engine
RandomEngine g_rnEngine = createInitialEngine();

}  // namespace

namespace Random {

int range(int min, int max) {
    assert(min <= max);
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(g_rnEngine);
}

unsigned int range(unsigned int min, unsigned int max) {
    assert(min <= max);
    std::uniform_int_distribution<unsigned int> distribution(min, max);
    return distribution(g_rnEngine);
}

float range(float min, float max) {
    assert(min <= max);
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(g_rnEngine);
}

void setSeed(unsigned long seed) {
    g_rnEngine.seed(seed);
}

Color color() {
    Color color;
    color.r = range(0.0f,1.0f);
    color.g = range(0.0f,1.0f);
    color.b = range(0.0f,1.0f);
    return color;
}

Color color(const Color& color1, const Color& color2) {
    Color color;
    color.r = range(color1.r, color2.r);
    color.g = range(color1.g, color2.g);
    color.b = range(color1.b, color2.b);
    color.a = range(color1.a, color2.a);
    return color;
}

}  // namespace Random
}  // namespace mahi::gui
