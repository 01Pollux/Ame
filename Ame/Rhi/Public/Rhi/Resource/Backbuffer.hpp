#pragma once

#include <Core/Ame.hpp>
#include <Math/Colors.hpp>

#include <Rhi/Resource/Texture.hpp>
#include <Rhi/Resource/View.hpp>

namespace Ame::Rhi
{
    struct Backbuffer
    {
        Texture      Resource;
        ResourceView View;
    };

    enum class BackbufferClearType : uint8_t
    {
        None,
        Color,
    };
} // namespace Ame::Rhi