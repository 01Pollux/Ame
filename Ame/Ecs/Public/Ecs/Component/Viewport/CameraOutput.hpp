#pragma once

#include <Rhi/Resource/Texture.hpp>

namespace Ame::Ecs::Component
{
    struct CameraOutput
    {
        String                  SourceView;
        Ptr<Rhi::ScopedTexture> OutputTexture;
        bool                    OutputToBackbuffer = true;
    };
} // namespace Ame::Ecs::Component