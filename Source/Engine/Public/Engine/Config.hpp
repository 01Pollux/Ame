#pragma once

#include <Core/Ame.hpp>
#include <Module/Core/Config.hpp>
#include <Module/Rhi/Config.hpp>
#include <Module/Graphics/Config.hpp>

namespace Ame
{
    struct EngineConfig
    {
        CoreModuleConfig          CoreConfig;
        Opt<RhiModuleConfig>      RhiConfig;
        Opt<GraphicsModuleConfig> GraphicsConfig;
    };
} // namespace Ame
