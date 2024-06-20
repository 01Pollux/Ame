#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    Buffer::Buffer(
        nri::CoreInterface& nriCore,
        nri::Buffer*        buffer,
        MemoryLocation      location) :
        m_NriCore(&nriCore),
        m_Buffer(buffer)
    {
        if (location == MemoryLocation::HOST_UPLOAD ||
            location == MemoryLocation::HOST_READBACK)
        {
            m_Mapped = nriCore.MapBuffer(*m_Buffer, 0, nri::WHOLE_SIZE);
        }
    }

    //

    void Buffer::SetName(
        const char* name) const
    {
        m_NriCore->SetBufferDebugName(*m_Buffer, name);
    }

    const BufferDesc& Buffer::GetDesc() const
    {
        return m_NriCore->GetBufferDesc(*m_Buffer);
    }

    nri::Buffer* const& Buffer::Unwrap() const
    {
        return m_Buffer;
    }

    void* Buffer::GetNative() const
    {
        return std::bit_cast<void*>(m_NriCore->GetBufferNativeObject(*m_Buffer));
    }

    //

    std::byte* Buffer::GetPtr(
        size_t offset)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_Mapped != nullptr, "Buffer is not host visible.");
        return static_cast<std::byte*>(m_Mapped) + offset;
    }

    const std::byte* Buffer::GetPtr(
        size_t offset) const
    {
        AME_LOG_ASSERT(Log::Rhi(), m_Mapped != nullptr, "Buffer is not host visible.");
        return static_cast<const std::byte*>(m_Mapped) + offset;
    }

    //

    void ScopedBuffer::Release()
    {
        m_Allocator->Release(*this).wait();
    }
} // namespace Ame::Rhi