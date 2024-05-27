#pragma once

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/Texture.hpp>

namespace Ame::Rhi
{
    struct BufferCopyDesc
    {
        Ref<const Buffer> RhiBuffer;
        size_t            Offset = 0;
    };

    struct TextureCopyDesc
    {
        Ref<const Texture>       RhiTexture;
        const TextureRegionDesc* Region = nullptr;
    };

    struct TransferCopyDesc
    {
        Ref<const Texture> RhiTexture;
        Ref<const Buffer>  RhiBuffer;

        TextureRegionDesc     Region;
        TextureDataLayoutDesc Layout;
    };
} // namespace Ame::Rhi