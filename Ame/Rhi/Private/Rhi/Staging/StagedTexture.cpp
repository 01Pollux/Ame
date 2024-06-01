#include <Rhi/Staging/StagedTexture.hpp>
#include <Rhi/Device/Device.hpp>
#include <Rhi/Util/ResourceSize.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi::Staging
{
    [[nodiscard]] static BufferUsageBits TextureUsageToBufferUsage(
        TextureUsageBits usage)
    {
        switch (usage)
        {
        case TextureUsageBits::SHADER_RESOURCE:
            return BufferUsageBits::SHADER_RESOURCE;
        case TextureUsageBits::SHADER_RESOURCE_STORAGE:
            return BufferUsageBits::SHADER_RESOURCE_STORAGE;
        }
        return BufferUsageBits::NONE;
    }

    StagedTexture::StagedTexture(
        Device&            rhiDevice,
        const TextureDesc& desc,
        StagedAccessType   accessType) :
        m_DeviceDesc(rhiDevice.GetDesc()),
        m_Desc(desc),
        m_TextureSize(
            Util::GetUploadBufferTextureSize(
                m_DeviceDesc,
                desc.format,
                desc.width,
                desc.height,
                desc.depth,
                desc.mipNum,
                1)),
        m_TextureBuffer(
            rhiDevice,
            StagedAccessToMemoryLocation(accessType),
            BufferDesc{
                .size      = m_TextureSize * desc.arraySize,
                .usageMask = TextureUsageToBufferUsage(desc.usageMask) }),
        m_Regions(CreateRegions(
            m_DeviceDesc,
            m_Desc,
            m_TextureSize))
    {
    }

    const TextureDesc& StagedTexture::GetTextureDesc() const
    {
        return m_Desc;
    }

    const Buffer& StagedTexture::GetBuffer() const
    {
        return m_TextureBuffer;
    }

    size_t StagedTexture::GetBufferSize() const
    {
        return m_TextureBuffer.GetDesc().size;
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

        region.Offset += Util::GetUploadBufferTextureFlatSize(m_DeviceDesc, m_Desc.format, actualRect.Position[0], actualRect.Position[1], actualRect.Position[2]);
        region.Width  = actualRect.Size[0];
        region.Height = actualRect.Size[1];
        region.Depth  = actualRect.Size[2];

        return region;
    }

    //

    auto StagedTexture::CreateRegions(
        const DeviceDesc&  deviceDesc,
        const TextureDesc& textureDesc,
        size_t             textureSize) -> StagedTextureRegionList
    {
        StagedTextureRegionList regions;
        regions.reserve(static_cast<size_t>(textureDesc.arraySize) * textureDesc.mipNum);

        auto& formatProps = GetFormatProps(textureDesc.format);

        size_t offset = 0;
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
                lastRegion.Offset += textureSize * i;
            }
        }

        return regions;
    }
} // namespace Ame::Rhi::Staging