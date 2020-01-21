#include "Utility/Range.hpp"

namespace carnot {
    generator<std::size_t> range(std::size_t last)
    {
        std::size_t first = 0;
        while (first != last)
        {
            co_yield first++;
        }
    }
}