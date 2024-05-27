#pragma once

#include <Rhi/Resource/Texture.hpp>
#include <Rhi/Resource/View.hpp>

namespace Ame::Ecs::Component
{
    struct CameraOutput
    {
        Ptr<Rhi::Texture> OutputTexture;
        String            SourceView;
    };
} // namespace Ame::Ecs::Component