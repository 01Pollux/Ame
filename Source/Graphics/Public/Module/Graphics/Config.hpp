#pragma once

#include <Core/Interface.hpp>
#include <Core/Ame.hpp>

#include <Window/Window.hpp>
#include <Window/WindowCreateDesc.hpp>
#include <Rhi/Device/RhiDeviceCreateDesc.hpp>

namespace Ame
{
    // {B8B1F60A-BFB3-424B-9AA4-1D2D844B091F}
    static const UId IID_GraphicsModule = { 0xb8b1f60a, 0xbfb3, 0x424b, { 0x9a, 0xa4, 0x1d, 0x2d, 0x84, 0x4b, 0x9, 0x1f } };

    struct GraphicsModuleConfig
    {
        Rhi::DeviceCreateDesc RhiDeviceDesc;
        bool                  EnableImGuiSubmodule : 1 = true;
    };

    class PlatformWindowSubmodule;
    class RhiGraphicsSubmodule;
    class ImGuiSubmodule;
} // namespace Ame