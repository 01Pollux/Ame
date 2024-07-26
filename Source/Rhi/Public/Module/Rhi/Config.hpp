#pragma once

#include <Core/Interface.hpp>
#include <Core/Ame.hpp>

#include <Window/Window.hpp>
#include <Window/WindowCreateDesc.hpp>

#include <Rhi/Device/RhiDeviceCreateDesc.hpp>
#include <Rhi/ImGui/ImGuiRendererCreateDesc.hpp>

namespace Ame
{
    // {B8B1F60A-BFB3-424B-9AA4-1D2D844B091F}
    static constexpr UId IID_RhiModule = { 0xb8b1f60a, 0xbfb3, 0x424b, { 0x9a, 0xa4, 0x1d, 0x2d, 0x84, 0x4b, 0x9, 0x1f } };
    class RhiModule;

    // {CF40EE16-0DE3-4095-938A-FB8993CD01FD}
    static constexpr UId IID_ImGuiSubmodule = { 0xcf40ee16, 0xde3, 0x4095, { 0x93, 0x8a, 0xfb, 0x89, 0x93, 0xcd, 0x1, 0xfd } };
    class ImGuiSubmodule;

    // {640F1A0F-8EBC-4722-931D-92EAD1EFFE74}
    static constexpr UId IID_PlatformWindowSubmodule = { 0x640f1a0f, 0x8ebc, 0x4722, { 0x93, 0x1d, 0x92, 0xea, 0xd1, 0xef, 0xfe, 0x74 } };
    class PlatformWindowSubmodule;

    // {116F0CD7-9584-417E-BA5E-5EC61E74C7E6}
    static constexpr UId IID_RhiGraphicsSubmodule = { 0x116f0cd7, 0x9584, 0x417e, { 0xba, 0x5e, 0x5e, 0xc6, 0x1e, 0x74, 0xc7, 0xe6 } };
    class RhiGraphicsSubmodule;

    struct GraphicsModuleImGuiConfig
    {
        static constexpr uint32_t DefaultInitialVBSize = 1024;
        static constexpr uint32_t DefaultInitialIBSize = 2048;

        uint32_t InitialVertexBufferSize = DefaultInitialVBSize;
        uint32_t InitialIndexBufferSize  = DefaultInitialIBSize;

        Rhi::ImGuiColorConversionMode ConversionMode = Rhi::ImGuiColorConversionMode::Auto;

        /// <summary>
        /// If true, the renderer will be created in a multi-threaded mode, and only one instance is allowed to execute at a time.
        /// (Between BeginFrame and EndFrame)
        /// </summary>
        bool MultiThreaded        : 1 = true;
        bool EnableImGuiSubmodule : 1 = true;
    };

    struct RhiModuleConfig
    {
        Rhi::DeviceCreateDesc     RhiDeviceDesc;
        GraphicsModuleImGuiConfig ImGuiConfig;
    };
} // namespace Ame