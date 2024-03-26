#include <Core/Random.hpp>

namespace Ame::Random
{
    static std::mt19937_64 s_RandomEngine;

    void SetSeed(
        uint64_t seed)
    {
        s_RandomEngine.seed(seed);
    }

    std::mt19937_64& GetEngine()
    {
        return s_RandomEngine;
    }
} // namespace Ame::Random