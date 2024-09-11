#pragma once

#include <Core/Interface.hpp>
#include <Plugin/ModuleRegistry.hpp>

namespace Ame::Interfaces
{
    struct AME_ENGINE_API GraphicsModuleConfig
    {
        void ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner = nullptr) const;
    };
} // namespace Ame::Interfaces