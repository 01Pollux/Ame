#pragma once

#include <Core/Subsystem.hpp>

#include <Gfx/Renderer.hpp>

#include <Frame/Subsystem/Frame.hpp>
#include <Frame/Subsystem/Timer.hpp>
#include <Rhi/Subsystem/Device.hpp>
#include <Ecs/Subsystem/Universe.hpp>

namespace Ame::Gfx
{
    struct RendererSubsystem : AutoWiredSingleSubsystem<Renderer>, kgr::final
    {
    };

    auto service_map(const Renderer&) -> RendererSubsystem;
} // namespace Ame::Gfx
