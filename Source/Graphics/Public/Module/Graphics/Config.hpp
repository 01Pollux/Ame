#pragma once

#include <Core/Interface.hpp>
#include <Core/Ame.hpp>

namespace Ame
{
    // {31954197-C7A3-430C-B0D2-41C5DA607EE9}
    static constexpr UId IID_GraphicsModule = { 0x31954197, 0xc7a3, 0x430c, { 0xb0, 0xd2, 0x41, 0xc5, 0xda, 0x60, 0x7e, 0xe9 } };
    class GraphicsModule;

    // {1EE289FB-1A56-4AC1-9D2B-7CF354B9A9BE}
    static constexpr UId IID_RendererSubmodule = { 0x1ee289fb, 0x1a56, 0x4ac1, { 0x9d, 0x2b, 0x7c, 0xf3, 0x54, 0xb9, 0xa9, 0xbe } };
	class RendererSubmodule;

    struct GraphicsModuleConfig
    {
        uint32_t SyncInterval = 0; // 0 = no vsync
    };
} // namespace Ame