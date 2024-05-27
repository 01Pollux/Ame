#pragma once

#include <Rhi/Resource/Texture.hpp>

namespace Ame::Ecs::Component
{
    struct CameraOutput
    {
        String            SourceView;
        Ptr<Rhi::Texture> OutputTexture;
    };
} // namespace Ame::Ecs::Component