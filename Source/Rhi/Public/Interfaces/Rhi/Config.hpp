#pragma once

#include <Core/Interface.hpp>
#include <Plugin/ModuleRegistry.hpp>

#include <Window/Window.hpp>
#include <Window/WindowCreateDesc.hpp>

#include <Rhi/Device/RhiDeviceCreateDesc.hpp>
#include <Rhi/ImGui/ImGuiRendererCreateDesc.hpp>

namespace Ame::Interfaces
{
    // {CF40EE16-0DE3-4095-938A-FB8993CD01FD}
    inline constexpr UId IID_ImGuiBackend{
        0xcf40ee16, 0xde3, 0x4095, { 0x93, 0x8a, 0xfb, 0x89, 0x93, 0xcd, 0x1, 0xfd }
    };
    class ImGuiBackend;

    // {640F1A0F-8EBC-4722-931D-92EAD1EFFE74}
    inline constexpr UId IID_WindowBackend{
        0x640f1a0f, 0x8ebc, 0x4722, { 0x93, 0x1d, 0x92, 0xea, 0xd1, 0xef, 0xfe, 0x74 }
    };
    class WindowBackend;

    // {116F0CD7-9584-417E-BA5E-5EC61E74C7E6}
    inline constexpr UId IID_RhiBackend{
        0x116f0cd7, 0x9584, 0x417e, { 0xba, 0x5e, 0x5e, 0xc6, 0x1e, 0x74, 0xc7, 0xe6 }
    };
    class RhiBackend;

    struct GraphicsModuleImGuiConfig
    {
        static constexpr uint32_t DefaultInitialVBSize = 1024;
        static constexpr uint32_t DefaultInitialIBSize = 2048;

        uint32_t InitialVertexBufferSize = DefaultInitialVBSize;
        uint32_t InitialIndexBufferSize  = DefaultInitialIBSize;

        Rhi::ImGuiColorConversionMode ConversionMode = Rhi::ImGuiColorConversionMode::Auto;

        /// <summary>
        /// If true, the renderer will be created in a multi-threaded mode, and only one instance is allowed to execute
        /// at a time. (Between BeginFrame and EndFrame)
        /// </summary>
        bool MultiThreaded        : 1 = true;
        bool EnableImGuiSubmodule : 1 = true;
    };

    struct RhiModuleConfig
    {
        Rhi::DeviceCreateDesc     RhiDeviceDesc;
        GraphicsModuleImGuiConfig ImGuiConfig;

        void ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner = nullptr) const;
    };
} // namespace Ame::Interfaces