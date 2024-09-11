#pragma once

#include <Interfaces/Core/Config.hpp>
#include <Interfaces/Rhi/Config.hpp>
#include <Interfaces/Ecs/Config.hpp>
#include <Interfaces/Graphics/Config.hpp>

namespace Ame
{
    struct AME_ENGINE_API EngineConfig
    {
        Interfaces::CoreModuleConfig          CoreConfig;
        Opt<Interfaces::RhiModuleConfig>      RhiConfig;
        Opt<Interfaces::EcsModuleConfig>      EcsConfig;
        Opt<Interfaces::GraphicsModuleConfig> GraphicsConfig;

        void ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner) const;
    };
} // namespace Ame
