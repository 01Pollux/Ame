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

    class PlatformWindowSubmodule;
    class RhiGraphicsSubmodule;
    class ImGuiSubmodule;
} // namespace Ame