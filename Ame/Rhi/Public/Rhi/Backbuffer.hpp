#pragma once

#include <Core/Ame.hpp>
#include <Rhi/Texture.hpp>
#include <Rhi/View.hpp>
#include <Math/Colors.hpp>

namespace Ame::Rhi
{
    struct Backbuffer
    {
        Texture                  Resource;
        RenderTargetResourceView View;
    };

    enum class BackbufferClearType : uint8_t
    {
        None,
        Color,
    };
} // namespace Ame::Rhi