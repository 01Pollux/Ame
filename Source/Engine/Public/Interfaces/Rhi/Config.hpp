#pragma once

#include <Core/Interface.hpp>
#include <Plugin/ModuleRegistry.hpp>

#include <Window/Window.hpp>
#include <Window/WindowCreateDesc.hpp>

#include <Rhi/Device/RhiDeviceCreateDesc.hpp>
#include <Rhi/ImGui/ImGuiRendererCreateDesc.hpp>

namespace Ame::Interfaces
{
    struct GraphicsModuleImGuiConfig
    {
        static constexpr uint32_t DefaultInitialVBSize = 1024;
        static constexpr uint32_t DefaultInitialIBSize = 2048;

        uint32_t InitialVertexBufferSize = DefaultInitialVBSize;
        uint32_t InitialIndexBufferSize  = DefaultInitialIBSize;

        Rhi::ImGuiColorConversionMode ConversionMode = Rhi::ImGuiColorConversionMode::Auto;

        bool EnableImGuiSubmodule : 1 = true;
    };

    struct AME_ENGINE_API RhiModuleConfig
    {
        Rhi::DeviceCreateDesc     RhiDeviceDesc;
        GraphicsModuleImGuiConfig ImGuiConfig;

        void ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner = nullptr) const;
    };
} // namespace Ame::Interfaces