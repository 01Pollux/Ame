#include <Rhi/Util/ResourceSize.hpp>
#include <Rhi/Device/Device.hpp>
#include <Math/Common.hpp>

namespace Ame::Rhi::Util
{
    size_t GetUploadBufferTextureSize(
        const DeviceDesc&  desc,
        const TextureDesc& textureDesc,
        size_t             instanceCount) noexcept
    {

        uint32_t mipCount  = textureDesc.mipNum;
        uint32_t arraySize = textureDesc.arraySize;

        if (!mipCount)
        {
            mipCount = 1;
        }
        if (!arraySize)
        {
            arraySize = 1;
        }

        auto& formatProps = GetFormatProps(textureDesc.format);

        uint32_t width  = std::max(static_cast<uint32_t>(textureDesc.width), 1u) * formatProps.stride * formatProps.blockWidth;
        uint32_t height = std::max(static_cast<uint32_t>(textureDesc.height), 1u) * formatProps.blockHeight;
        uint32_t depth  = std::max(static_cast<uint32_t>(textureDesc.depth), 1u);

        size_t size = 0;
        for (uint32_t i = 0; i < mipCount; ++i)
        {
            size += GetUploadBufferTextureSizeAt(desc, width, height, depth);

            width  = std::max(width >> 1, 1u);
            height = std::max(height >> 1, 1u);
            depth  = std::max(depth >> 1, 1u);
        }
        return instanceCount * size * arraySize;
    }

    size_t GetUploadBufferTextureSizeAt(
        const DeviceDesc& desc,
        uint32_t          width,
        uint32_t          height,
        uint32_t          depth) noexcept
    {
        size_t size = Math::AlignUp(width, desc.uploadBufferTextureRowAlignment);
        size        = Math::AlignUp(size * height, desc.uploadBufferTextureSliceAlignment);
        size        = size * depth;
        return size;
    }

    size_t GetUploadBufferTextureSizeAt(
        const DeviceDesc& desc,
        uint32_t width, 
        uint32_t height,
        uint32_t depth, 
        uint32_t mipIndex) noexcept
    {
        width  = std::max(width >> mipIndex, 1u);
		height = std::max(height >> mipIndex, 1u);
		depth  = std::max(depth >> mipIndex, 1u);
		return GetUploadBufferTextureSizeAt(desc, width, height, depth);
    }

    size_t GetUploadBufferTextureRowSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount) noexcept
    {
        return instanceCount * Math::AlignUp(structSize, desc.uploadBufferTextureRowAlignment);
    }

    size_t GetUploadBufferTextureSliceSize(
        const DeviceDesc& desc,
        size_t            structSize,
        size_t            instanceCount) noexcept
    {
        return instanceCount * Math::AlignUp(structSize, desc.uploadBufferTextureSliceAlignment);
    }

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