#pragma once

#include <Ecs/Core.hpp>

namespace Ame::EcsUtil
{
    /// <summary>
    /// Get unique entity name that is not already in use.
    /// </summary>
    [[nodiscard]] StringU8 GetUniqueEntityName(
        const flecs::world&  FlecsWorld,
        const char*          Name,
        const flecs::entity& FlecsParent = flecs::entity::null());
} // namespace Ame::EcsUtil