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
        m_TextureBuffer(
            rhiDevice,
            StagedAccessToMemoryLocation(accessType),
            BufferDesc{
                .size      = Util::GetUploadBufferTextureSize(m_DeviceDesc, desc),
                .usageMask = TextureUsageToBufferUsage(desc.usageMask) })
    {
    }

    const Buffer& StagedTexture::GetBuffer() const
    {
        return m_TextureBuffer;
    }

    size_t StagedTexture::GetBufferSize() const
    {
        return m_TextureBuffer.GetDesc().size;
    }

    BufferRange Staging::StagedTexture::GetRegion(
        uint32_t                mipLevel,
        uint32_t                arrayIndex,
        TextureRect::Coordinate position) const
    {
        auto& formatProps = GetFormatProps(m_Desc.format);

        uint32_t width  = std::max(static_cast<uint32_t>(m_Desc.width), 1u) * formatProps.stride * formatProps.blockWidth;
        uint32_t height = std::max(static_cast<uint32_t>(m_Desc.height), 1u) * formatProps.blockHeight;
        uint32_t depth  = std::max(static_cast<uint32_t>(m_Desc.depth), 1u);

        size_t offset = Util::GetUploadBufferTextureSizeAt(
            m_DeviceDesc,
            width,
            height,
            depth,
            mipLevel);
        size_t sizeLeft = m_TextureBuffer.GetDesc().size - offset;

        return BufferRange(offset, sizeLeft);
    }
} // namespace Ame::Rhi::Staging