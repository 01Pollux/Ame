#include <Rhi/Util/ResourceSize.hpp>
#include <Rhi/Device/Device.hpp>
#include <Math/Common.hpp>

namespace Ame::Rhi::Util
{
    size_t GetUploadBufferTextureSize(
        const DeviceDesc& desc,
        ResourceFormat    format,
        uint32_t          width,
        uint32_t          height,
        uint32_t          depth,
        uint32_t          mipCount,
        uint32_t          arraySize) noexcept
    {
        auto& formatProps = GetFormatProps(format);

        size_t size = 0;
        for (uint32_t i = 0; i < mipCount; ++i)
        {
            size += GetUploadBufferTextureFlatSize(desc, format, width, height, depth);

            width  = GetTextureDimension(width, 1);
            height = GetTextureDimension(height, 1);
            depth  = GetTextureDimension(depth, 1);
        }
        return size * arraySize;
    }

    Dim_t GetTextureDimension(
        Dim_t dimension,
        Mip_t mipIndex) noexcept
    {
        return std::max(dimension >> mipIndex, 1);
    }

    //

    size_t GetUploadBufferTextureFlatSize(
        const DeviceDesc& desc,
        ResourceFormat    format,
        uint32_t          width,
        uint32_t          height,
        uint32_t          depth) noexcept
    {
        auto& formatProps = GetFormatProps(format);

        width *= formatProps.stride * formatProps.blockWidth;
        height *= formatProps.blockHeight;

        size_t rowPitch   = Math::AlignUp(width, desc.uploadBufferTextureRowAlignment);
        size_t slicePitch = Math::AlignUp(height * rowPitch, desc.uploadBufferTextureSliceAlignment);
        return slicePitch * depth;
    }

    size_t GetUploadBufferTextureSizeAt(
        const DeviceDesc& desc,
        ResourceFormat    format,
        uint32_t          width,
        uint32_t          height,
        uint32_t          depth,
        uint32_t          mipIndex,
        uint32_t          arrayIndex) noexcept
    {
        uint32_t w      = GetTextureDimension(width, mipIndex);
        uint32_t h      = GetTextureDimension(height, mipIndex);
        uint32_t d      = GetTextureDimension(depth, mipIndex);
        size_t   offset = arrayIndex * GetUploadBufferTextureFlatSize(desc, format, width, height, depth);
        return offset + GetUploadBufferTextureFlatSize(desc, format, w, h, d);
    }

    //

    uint32_t GetUploadBufferTextureRowSize(
        const DeviceDesc& desc,
        ResourceFormat    format,
        uint32_t          width) noexcept
    {
        auto& formatProps = GetFormatProps(format);
        width *= formatProps.stride * formatProps.blockWidth;
        return Math::AlignUp(width, desc.uploadBufferTextureRowAlignment);
    }

    uint32_t GetUploadBufferTextureSliceSize(
        const DeviceDesc& desc,
        ResourceFormat    format,
        uint32_t          rowSize,
        uint32_t          heigth) noexcept
    {
        auto& formatProps = GetFormatProps(format);
        heigth *= formatProps.blockHeight;
        return Math::AlignUp(heigth * rowSize, desc.uploadBufferTextureSliceAlignment);
    }

    //

    size_t GetTypedBufferSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount) noexcept
    {
        return instanceCount * Math::AlignUp(structSize, desc.typedBufferOffsetAlignment);
    }

    size_t GetConstantBufferSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount) noexcept
    {
        return instanceCount * Math::AlignUp(structSize, desc.constantBufferOffsetAlignment);
    }

    size_t GetUnorderedAccessBufferSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount) noexcept
    {
        return instanceCount * Math::AlignUp(structSize, desc.storageBufferOffsetAlignment);
    }
} // namespace Ame::Rhi::Util