#include <Rhi/Staging/StagedTexture.hpp>
#include <Rhi/Descs/View.hpp>

#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Util/ResourceSize.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi::Staging
{
    [[nodiscard]] static BufferUsageBits TextureUsageToBufferUsage(
        TextureUsageBits usage)
    {
        BufferUsageBits bufferUsage = BufferUsageBits::NONE;
        if ((usage & TextureUsageBits::SHADER_RESOURCE))
        {
            bufferUsage |= BufferUsageBits::SHADER_RESOURCE;
        }
        if ((usage & TextureUsageBits::SHADER_RESOURCE_STORAGE))
        {
            bufferUsage |= BufferUsageBits::SHADER_RESOURCE_STORAGE;
        }
        return bufferUsage;
    }

    //

    StagedTexture StagedTexture::Create(
        DeviceResourceAllocator& allocator,
        const TextureDesc&       desc,
        StagedAccessType         accessType)
    {
        auto& deviceDesc = allocator.GetDeviceDesc();
        auto  buffer     = allocator.CreateBuffer(
            { .size      = GetTextureSize(allocator.GetDeviceDesc(), desc),
                   .usageMask = TextureUsageToBufferUsage(desc.usageMask) },
            StagedAccessToMemoryLocation(accessType));

        size_t totalSize = buffer.GetDesc().size;
        return StagedTexture(deviceDesc, desc, std::move(buffer), totalSize, 0);
    }

    StagedTexture StagedTexture::CreateForView(
        const Rhi::DeviceDesc& deviceDesc,
        const TextureDesc&     desc,
        Buffer                 buffer,
        size_t                 totalSize,
        size_t                 offset)
    {
        return StagedTexture(deviceDesc, desc, std::move(buffer), totalSize, offset);
    }

    //

    StagedTexture::StagedTexture(
        const Rhi::DeviceDesc& deviceDesc,
        const TextureDesc&     desc,
        Buffer                 buffer,
        size_t                 totalSize,
        size_t                 offset) :
        Buffer(std::move(buffer)),
        m_DeviceDesc(&deviceDesc),
        m_Desc(desc),
        m_Regions(CreateRegions(deviceDesc, m_Desc, totalSize, offset))
    {
    }

    //

    size_t StagedTexture::GetTextureSize(
        const DeviceDesc&  deviceDesc,
        const TextureDesc& textureDesc)
    {
        return Util::GetUploadBufferTextureSize(
            deviceDesc,
            textureDesc.format,
            textureDesc.width,
            textureDesc.height,
            textureDesc.depth,
            textureDesc.mipNum,
            textureDesc.arraySize);
    }

    //

    const TextureDesc& StagedTexture::GetTextureDesc() const
    {
        return m_Desc;
    }

    size_t StagedTexture::GetBufferSize() const
    {
        return GetDesc().size;
    }

    const StagedTextureRegion& StagedTexture::GetRegion(
        uint32_t mipLevel,
        uint32_t arrayIndex) const
    {
        AME_LOG_ASSERT(Log::Rhi(), mipLevel < m_Desc.mipNum, "Mip level out of range");
        AME_LOG_ASSERT(Log::Rhi(), arrayIndex < m_Desc.arraySize, "Array index out of range");
        return m_Regions[mipLevel + static_cast<size_t>(arrayIndex) * m_Desc.mipNum];
    }

    StagedTextureRegion StagedTexture::GetRegion(
        const TextureRect& rect,
        uint32_t           mipLevel,
        uint32_t           arrayIndex) const
    {
        auto actualRect = rect.Transform(mipLevel, m_Desc);
        auto region     = GetRegion(mipLevel, arrayIndex);

        AME_LOG_ASSERT(Log::Rhi(), actualRect.Size[0] <= region.Width, "Rect width out of range");
        AME_LOG_ASSERT(Log::Rhi(), actualRect.Size[1] <= region.Height, "Rect height out of range");
        AME_LOG_ASSERT(Log::Rhi(), actualRect.Size[2] <= region.Depth, "Rect depth out of range");

        region.Offset += Util::GetUploadBufferTextureFlatSize(*m_DeviceDesc, m_Desc.format, actualRect.Position[0], actualRect.Position[1], actualRect.Position[2]);
        region.Width  = actualRect.Size[0];
        region.Height = actualRect.Size[1];
        region.Depth  = actualRect.Size[2];

        return region;
    }

    //

    auto StagedTexture::CreateRegions(
        const DeviceDesc&  deviceDesc,
        const TextureDesc& textureDesc,
        size_t             totalSize,
        size_t             relativeOffset) -> StagedTextureRegionList
    {
        totalSize /= textureDesc.arraySize;

        StagedTextureRegionList regions;
        regions.reserve(static_cast<size_t>(textureDesc.arraySize) * textureDesc.mipNum);

        auto& formatProps = GetFormatProps(textureDesc.format);

        size_t offset = relativeOffset;
        for (uint32_t i = 0; i < textureDesc.mipNum; i++)
        {
            Dim_t    width    = Util::GetTextureDimension(textureDesc.width, i);
            Dim_t    height   = Util::GetTextureDimension(textureDesc.height, i);
            Dim_t    depth    = Util::GetTextureDimension(textureDesc.depth, i);
            uint32_t rowPitch = Util::GetUploadBufferTextureRowSize(deviceDesc, textureDesc.format, width);

            regions.emplace_back(
                StagedTextureRegion{
                    .Props    = formatProps,
                    .Offset   = offset,
                    .RowPitch = rowPitch,
                    .Width    = width,
                    .Height   = height,
                    .Depth    = depth });

            offset += Util::GetUploadBufferTextureFlatSize(deviceDesc, textureDesc.format, width, height, depth);
        }

        for (uint32_t i = 1; i < textureDesc.arraySize; i++)
        {
            for (uint32_t j = 0; j < textureDesc.mipNum; j++)
            {
                auto& lastRegion = regions.emplace_back(regions[j]);
                lastRegion.Offset += totalSize * i;
            }
        }

        return regions;
    }
} // namespace Ame::Rhi::Staging