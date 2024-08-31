#pragma once

#include <Interfaces/Core/Config.hpp>
#include <Interfaces/Rhi/Config.hpp>
#include <Interfaces/Ecs/Config.hpp>
#include <Interfaces/Graphics/Config.hpp>

namespace Ame
{
    struct EngineConfig
    {
        Interfaces::CoreModuleConfig          CoreConfig;
        Opt<Interfaces::RhiModuleConfig>      RhiConfig;
        Opt<Interfaces::EcsModuleConfig>      EcsConfig;
        Opt<Interfaces::GraphicsModuleConfig> GraphicsConfig;

        void ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner) const
        {
            CoreConfig.ExposeInterfaces(registry, owner);
            if (RhiConfig)
            {
                RhiConfig->ExposeInterfaces(registry, owner);
            }
            if (EcsConfig)
            {
                EcsConfig->ExposeInterfaces(registry, owner);
            }
            if (GraphicsConfig)
            {
                GraphicsConfig->ExposeInterfaces(registry, owner);
            }
        }
    };
} // namespace Ame
