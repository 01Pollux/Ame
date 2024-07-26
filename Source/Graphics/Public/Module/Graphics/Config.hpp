#pragma once

#include <Core/Interface.hpp>
#include <Core/Ame.hpp>

namespace Ame
{
    // {31954197-C7A3-430C-B0D2-41C5DA607EE9}
    static constexpr UId IID_GraphicsModule = { 0x31954197, 0xc7a3, 0x430c, { 0xb0, 0xd2, 0x41, 0xc5, 0xda, 0x60, 0x7e, 0xe9 } };

    struct GraphicsModuleConfig
    {
        uint32_t SyncInterval = 0; // 0 = no vsync
    };

    class RendererSubmodule;
} // namespace Ame