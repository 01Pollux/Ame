#pragma once

#include <Rhi/Device/RhiDevice.hpp>

namespace Ame::Ecs
{
    struct CameraOutputComponent
    {
        String            SourceView = "_Image";
        Ptr<Dg::ITexture> OutputTexture;
    };
} // namespace Ame::Ecs