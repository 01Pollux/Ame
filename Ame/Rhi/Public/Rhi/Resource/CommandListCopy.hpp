#pragma once

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/Texture.hpp>

namespace Ame::Rhi
{
    struct BufferCopyDesc
    {
        const Buffer& RhiBuffer;
        size_t        Offset = 0;
    };

    struct TextureCopyDesc
    {
        const Texture&           RhiTexture;
        const TextureRegionDesc* Region = nullptr;
    };

    struct TransferCopyDesc
    {
        const Texture& RhiTexture;
        const Buffer&  RhiBuffer;

        TextureRegionDesc     Region;
        TextureDataLayoutDesc Layout;
    };
} // namespace Ame::Rhi