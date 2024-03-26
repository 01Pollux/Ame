#pragma once

#include <random>

namespace Ame::Random
{
    /// <summary>
    /// Seeds the random number generator.
    /// </summary>
    void SetSeed(
        uint64_t Seed);

    /// <summary>
    /// Gets the random number generator engine.
    /// </summary>
    [[nodiscard]] std::mt19937_64& GetEngine();
} // namespace Ame::Random