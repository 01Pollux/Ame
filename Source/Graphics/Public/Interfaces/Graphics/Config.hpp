#pragma once

#include <Core/Interface.hpp>
#include <Plugin/ModuleRegistry.hpp>

namespace Ame::Interfaces
{
    // {1EE289FB-1A56-4AC1-9D2B-7CF354B9A9BE}
    inline constexpr UId IID_Renderer{ 0x1ee289fb, 0x1a56, 0x4ac1, { 0x9d, 0x2b, 0x7c, 0xf3, 0x54, 0xb9, 0xa9, 0xbe } };
    class IRenderer;

    struct GraphicsModuleConfig
    {
        void ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner = nullptr) const;
    };
} // namespace Ame::Interfaces