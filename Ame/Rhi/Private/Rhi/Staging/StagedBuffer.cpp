#include <Rhi/Staging/StagedBuffer.hpp>

#include <Rhi/Device/ResourceAllocator.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi::Staging
{
    StagedBuffer StagedBuffer::Create(
        DeviceResourceAllocator& allocator,
        StagedAccessType         accessType,
        const BufferDesc&        desc)
    {
        auto buffer = allocator.CreateBuffer(
            desc,
            StagedAccessToMemoryLocation(accessType));

        size_t size = buffer.GetDesc().size;
        return StagedBuffer(std::move(buffer), 0, size);
    }

    StagedBuffer StagedBuffer::CreateForView(
        Buffer buffer,
        size_t offset,
        size_t size)
    {
        return StagedBuffer(std::move(buffer), offset, size);
    }

    //

    StagedBuffer::StagedBuffer(
        Buffer buffer,
        size_t offset,
        size_t size) :
        Buffer(std::move(buffer)),
        m_Offset(offset)
    {
    }

    //

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
        return Buffer::GetPtr(m_Offset + offset);
    }

    std::byte* StagedBuffer::GetPtr(
        size_t offset)
    {
        return Buffer::GetPtr(m_Offset + offset);
    }
} // namespace Ame::Rhi::Staging