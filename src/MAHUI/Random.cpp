#include <Utility/Random.hpp>
#include <SFML/Config.hpp>
#include <cassert>
#include <ctime>
#include <random>

namespace carnot {
namespace {

#ifdef CARNOT_USE_STD_RANDOMENGINE

// Use Mersenne Twister as default standard random engine
typedef std::mt19937 Engine;

#else

// Random generator engine (Multiply With Carry)
// Many thanks to Volkard Henkel for the implementation.
class RandomEngine {
public:
    // Type definition for usage inside Std.Random
    typedef sf::Uint32 result_type;

public:
    // Constructor
    explicit RandomEngine(sf::Uint32 seedVal) { seed(seedVal); }

    // Return random number
    sf::Uint32 operator()() {
        const sf::Uint64 a = 1967773755;

        x = a * (x & 0xffffffff) + (x >> 32);
        return static_cast<sf::Uint32>(x);
    }

    // set seed (compliant to Std.Random)
    void seed(sf::Uint32 seedVal = 0) { x = seedVal + !seedVal; }

    // Return minimal value (compliant to Std.Random)
    static constexpr sf::Uint32 min() { return 0; }

    // Return maximal value (compliant to Std.Random)
    static constexpr sf::Uint32 max() { return 0xffffffff; }

private:
    sf::Uint64 x;
};

#endif  // CARNOT_USE_STD_RANDOMENGINE

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
    color.r = static_cast<sf::Uint8>(range(0,255));
    color.g = static_cast<sf::Uint8>(range(0,255));
    color.b = static_cast<sf::Uint8>(range(0,255));
    return color;
}

Color color(const Color& color1, const Color& color2) {
    Color color;
    color.r = static_cast<sf::Uint8>(range(color1.r, color2.r));
    color.g = static_cast<sf::Uint8>(range(color1.g, color2.g));
    color.b = static_cast<sf::Uint8>(range(color1.b, color2.b));
    color.a = static_cast<sf::Uint8>(range(color1.a, color2.a));
    return color;
}

}  // namespace Random
}  // namespace carnot
