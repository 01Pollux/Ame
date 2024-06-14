#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Rhi/Device.hpp>
#include <Subsystem/Ecs/Universe.hpp>
#include <Subsystem/RG/RenderGraph.hpp>
#include <Subsystem/WorldNTree/WorldNTree.hpp>

#include <Gfx/Compositor/EntityCompositor.hpp>

namespace Ame::Gfx
{
    struct EntityCompositorSubsystem : SingleSubsystem<
                                           EntityCompositor,
                                           Dependency<
                                               Rhi::DeviceSubsystem,
                                               Ecs::UniverseSubsystem,
                                               RG::GraphSubsystem,
                                               Extensions::WorldOctTreeBoxSubsystem>>,
                                       kgr::final
    {
    };

    auto service_map(const EntityCompositor&) -> EntityCompositorSubsystem;
} // namespace Ame::Gfx
