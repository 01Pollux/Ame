#pragma once

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/Texture.hpp>

namespace Ame::Rhi
{
    struct BufferCopyDesc
    {
        nri::Buffer* NriBuffer;
        size_t       Offset = 0;
    };

    struct TextureCopyDesc
    {
        nri::Texture*                NriTexture;
        Opt<const TextureRegionDesc> Region;
    };

    struct TransferCopyDesc
    {
        nri::Texture* NriTexture;
        nri::Buffer*  NriBuffer;

        TextureRegionDesc TextureRegion{};
        size_t            BufferOffset = 0;
    };
} // namespace Ame::Rhi