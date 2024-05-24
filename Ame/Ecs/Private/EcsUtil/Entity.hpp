#pragma once

#include <Ecs/Core.hpp>

namespace Ame::EcsUtil
{
    /// <summary>
    /// Get unique entity name that is not already in use.
    /// </summary>
    [[nodiscard]] String GetUniqueEntityName(
        const flecs::world&  world,
        const char*          name,
        const flecs::entity& parent = flecs::entity::null());
} // namespace Ame::EcsUtil