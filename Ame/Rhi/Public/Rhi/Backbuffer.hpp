#pragma once

#include <Rhi/Texture.hpp>
#include <Rhi/View.hpp>

namespace Ame::Rhi
{
    struct Backbuffer
    {
        Texture                  Resource;
        RenderTargetResourceView View;
    };
} // namespace Ame::Rhi