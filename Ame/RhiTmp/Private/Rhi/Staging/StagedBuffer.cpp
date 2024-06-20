#include <Rhi/Staging/StagedBuffer.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi::Staging
{
    StagedBuffer::StagedBuffer(
        Device&           rhiDevice,
        StagedAccessType  accessType,
        const BufferDesc& desc) :
        m_Buffer(rhiDevice, StagedAccessToMemoryLocation(accessType), desc),
        m_Size(desc.size)
    {
    }

    StagedBuffer::StagedBuffer(
        Buffer buffer,
        size_t offset,
        size_t size) :
        m_Buffer(std::move(buffer)),
        m_Offset(offset)
    {
    }

    //

    const Buffer& StagedBuffer::GetBuffer() const
    {
        return m_Buffer;
    }

    size_t StagedBuffer::GetSize() const
    {
        return m_Size;
    }

    size_t StagedBuffer::GetOffset() const
    {
        return m_Offset;
    }

    const std::byte* StagedBuffer::GetPtr(
        size_t offset) const
    {
        return m_Buffer.GetPtr(m_Offset + offset);
    }

    std::byte* StagedBuffer::GetPtr(
        size_t offset)
    {
        return m_Buffer.GetPtr(m_Offset + offset);
    }
} // namespace Ame::Rhi::Staging