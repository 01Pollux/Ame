#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device/Device.Impl.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    Buffer::Buffer(
        DeviceResourceAllocator& allocator,
        nri::Buffer*             buffer,
        MemoryLocation           location) :
        m_Allocator(&allocator),
        m_Buffer(buffer)
    {
        if (location == MemoryLocation::HOST_UPLOAD ||
            location == MemoryLocation::HOST_READBACK)
        {
            auto& nriCore = m_Allocator->GetNriCore();
            m_Mapped      = nriCore.MapBuffer(*m_Buffer, 0, nri::WHOLE_SIZE);
        }
    }

    void Buffer::Release(
        bool defer)
    {
        if (m_Buffer)
        {
            if (defer)
            {
                auto& frameManager = m_Allocator->GetFrameManager();
                auto& frame        = frameManager.GetCurrentFrame();

                auto lock = m_Allocator->TryLock();
                frame.DeferRelease(*m_Buffer);
            }
            else
            {
                auto& nriCore = m_Allocator->GetNriCore();
                nriCore.DestroyBuffer(*m_Buffer);
            }
            m_Buffer = nullptr;
        }
    }

    //

    void Buffer::SetName(
        const char* name) const
    {
        auto& nriCore = m_Allocator->GetNriCore();
        nriCore.SetBufferDebugName(*m_Buffer, name);
    }

    const BufferDesc& Buffer::GetDesc() const
    {
        auto& nriCore = m_Allocator->GetNriCore();
        return nriCore.GetBufferDesc(*m_Buffer);
    }

    nri::Buffer* const& Buffer::Unwrap() const
    {
        return m_Buffer;
    }

    void* Buffer::GetNative() const
    {
        auto& nriCore = m_Allocator->GetNriCore();
        return std::bit_cast<void*>(nriCore.GetBufferNativeObject(*m_Buffer));
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
} // namespace Ame::Rhi