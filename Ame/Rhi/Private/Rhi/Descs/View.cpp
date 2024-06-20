#include <Rhi/Descs/View.hpp>

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/Texture.hpp>

namespace Ame::Rhi
{
    BufferRange BufferRange::Transform(
        const Buffer& buffer) const noexcept
    {
        return Transform(buffer.GetDesc());
    }

    BufferRange BufferRange::Transform(
        const BufferDesc& bufferDesc) const noexcept
    {
        auto range = *this;
        if (range.Size == c_RemainingSize<size_t>)
        {
            range.Size = bufferDesc.size - range.Offset;
        }
        return range;
    }

    //

    MipLevel MipLevel::Transform(
        const Texture& texture) const noexcept
    {
        return Transform(texture.GetDesc());
    }

    MipLevel MipLevel::Transform(
        const TextureDesc& textureDesc) const noexcept
    {
        auto mipLevel = *this;
        if (mipLevel.Count == c_RemainingSize<Mip_t>)
        {
            mipLevel.Count = textureDesc.mipNum - mipLevel.Offset;
        }
        return mipLevel;
    }

    ArraySlice ArraySlice::Transform(
        const Texture& texture) const noexcept
    {
        return Transform(texture.GetDesc());
    }

    ArraySlice ArraySlice::Transform(
        const TextureDesc& textureDesc) const noexcept
    {
        auto arraySlice = *this;
        if (arraySlice.Count == c_RemainingSize<Dim_t>)
        {
            arraySlice.Count = textureDesc.arraySize - arraySlice.Offset;
        }
        return arraySlice;
    }

    TextureSubresource TextureSubresource::Transform(
        const Texture& texture) const noexcept
    {
        return Transform(texture.GetDesc());
    }

    TextureSubresource TextureSubresource::Transform(
        const TextureDesc& textureDesc) const noexcept
    {
        return TextureSubresource(Mips.Transform(textureDesc), Array.Transform(textureDesc));
    }

    //

    TextureRect TextureRect::Transform(
        uint32_t       mipIndex,
        const Texture& texture) const noexcept
    {
        return Transform(mipIndex, texture.GetDesc());
    }

    TextureRect TextureRect::Transform(
        uint32_t           mipIndex,
        const TextureDesc& textureDesc) const noexcept
    {
        auto rect = *this;
        if (rect.Size[0] == c_RemainingSize<Dim_t> ||
            rect.Size[1] == c_RemainingSize<Dim_t> ||
            rect.Size[2] == c_RemainingSize<Dim_t>)
        {
            if (rect.Size[0] == c_RemainingSize<Dim_t>)
            {
                rect.Size[0] = std::max((textureDesc.width - rect.Position[0]) >> mipIndex, 1);
            }
            if (rect.Size[1] == c_RemainingSize<Dim_t>)
            {
                rect.Size[1] = std::max((textureDesc.height - rect.Position[1]) >> mipIndex, 1);
            }
            if (rect.Size[2] == c_RemainingSize<Dim_t>)
            {
                rect.Size[2] = std::max((textureDesc.depth - rect.Position[2]) >> mipIndex, 1);
            }
        }
        return rect;
    }

    //

    BufferViewDesc BufferViewDesc::Transform(
        const Buffer& buffer) const noexcept
    {
        return Transform(buffer.GetDesc());
    }

    BufferViewDesc BufferViewDesc::Transform(
        const BufferDesc& bufferDesc) const noexcept
    {
        auto viewDesc  = *this;
        viewDesc.Range = Range.Transform(bufferDesc);
        return viewDesc;
    }

    //

    TextureViewDesc TextureViewDesc::Transform(
        const Texture& texture) const noexcept
    {
        return Transform(texture.GetDesc());
    }

    TextureViewDesc TextureViewDesc::Transform(
        const TextureDesc& textureDesc) const noexcept
    {
        auto viewDesc        = *this;
        viewDesc.Subresource = Subresource.Transform(textureDesc);
        if (Format == Rhi::ResourceFormat::UNKNOWN)
        {
            viewDesc.Format = textureDesc.format;
        }
        return viewDesc;
    }
} // namespace Ame::Rhi