#pragma once

#include <Core/Interface.hpp>
#include <Ecs/World.hpp>

namespace Ame::Interfaces
{
    struct EntityWorldCreateDesc;

    inline constexpr UId IID_EntityWorld = Ecs::IID_EntityWorld;
    using IEntityWorld                   = Ecs::World;

    [[nodiscard]] AME_ENGINE_API Ptr<IEntityWorld> CreateEntityWorld(Ptr<Rhi::IRhiDevice>         rhiDevice,
                                                                     const EntityWorldCreateDesc& createDesc);
} // namespace Ame::Interfaces