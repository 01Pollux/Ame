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
                .usageMask = TextureUsageToBufferUsage(desc.usageMask) })
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

    BufferRange StagedTexture::GetRegion(
        uint32_t    mipLevel,
        uint32_t    arrayIndex,
        TextureRect rect) const
    {
        auto& formatProps = GetFormatProps(m_Desc.format);
        auto  actualRect  = rect.Transform(mipLevel, m_Desc);

        size_t offset = m_TextureSize * arrayIndex +
                        Util::GetUploadBufferTextureFlatSize(m_DeviceDesc, m_Desc.format, actualRect.Position[0], actualRect.Position[1], actualRect.Position[2]);

        uint32_t width  = m_Desc.width;
        uint32_t height = m_Desc.height;
        uint32_t depth  = m_Desc.depth;

        for (uint32_t i = 0; i < mipLevel; i++)
        {
            offset += Util::GetUploadBufferTextureFlatSize(m_DeviceDesc, m_Desc.format, width, height, depth);

            width  = Util::GetTextureDimension(width, 1);
            height = Util::GetTextureDimension(height, 1);
            depth  = Util::GetTextureDimension(depth, 1);
        }

        size_t size = Util::GetUploadBufferTextureFlatSize(m_DeviceDesc, m_Desc.format, actualRect.Size[0], actualRect.Size[1], actualRect.Size[2]);

        return BufferRange(offset, size);
    }
} // namespace Ame::Rhi::Staging