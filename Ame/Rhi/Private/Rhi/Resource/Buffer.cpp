#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    Buffer::Buffer(
        Device&        RhiDevice,
        MemoryLocation Location,
        nri::Buffer*   NriBuffer) :
        m_Buffer(NriBuffer)
    {
        if (Location == MemoryLocation::HOST_UPLOAD || Location == MemoryLocation::HOST_READBACK)
        {
            auto& Impl    = RhiDevice.GetImpl();
            auto& Nri     = Impl.GetNRI();
            auto& NriCore = *Nri.GetCoreInterface();

            m_Mapped = NriCore.MapBuffer(*m_Buffer, 0, nri::WHOLE_SIZE);
        }
    }

    Buffer::Buffer(
        Device&           RhiDevice,
        MemoryLocation    Location,
        const BufferDesc& Desc) :
        Buffer(RhiDevice, Location, RhiDevice.Create(Location, Desc))
    {
    }

    void Buffer::Release(
        Device& RhiDevice,
        bool    Defer)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_Buffer != nullptr, "Buffer was already released.");

        if (m_Mapped)
        {
            auto& Impl    = RhiDevice.GetImpl();
            auto& Nri     = Impl.GetNRI();
            auto& NriCore = *Nri.GetCoreInterface();

            NriCore.UnmapBuffer(*m_Buffer);
            m_Mapped = nullptr;
        }

        RhiDevice.Release(*m_Buffer, Defer);
        m_Buffer = nullptr;
    }

    //

    nri::Buffer* Buffer::Unwrap(
        Device& RhiDevice) const
    {
        return m_Buffer;
    }

    void* Buffer::GetNative(
        Device& RhiDevice) const
    {
        auto& Impl    = RhiDevice.GetImpl();
        auto& Nri     = Impl.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        return std::bit_cast<void*>(NriCore.GetBufferNativeObject(*m_Buffer));
    }

    //

    void Buffer::SetName(
        Device&     RhiDevice,
        const char* Name)
    {
        auto& Impl    = RhiDevice.GetImpl();
        auto& Nri     = Impl.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.SetBufferDebugName(*m_Buffer, Name);
    }

    const BufferDesc& Buffer::GetDesc(
        Device& RhiDevice) const
    {
        auto& Impl    = RhiDevice.GetImpl();
        auto& Nri     = Impl.GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        return NriCore.GetBufferDesc(*m_Buffer);
    }

    //

    void* Buffer::GetPtr(
        size_t  Offset)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_Mapped != nullptr, "Buffer is not host visible.");
		return static_cast<uint8_t*>(m_Mapped) + Offset;
    }

    BufferResourceView Buffer::CreateView(
        Device&               RhiDevice,
        const BufferViewDesc& Desc) const
    {
        return BufferResourceView(RhiDevice.CreateView(*m_Buffer, Desc));
    }

    //

    nri::Buffer* Device::Create(
        MemoryLocation    Location,
        const BufferDesc& Desc)
    {
        auto& Nri     = m_Impl->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        auto NriBuffer = m_Impl->m_MemoryAllocator.CreateBuffer(Location, Desc);
        m_Impl->BeginTracking(NriBuffer, { nri::AccessBits::UNKNOWN, nri::StageBits::ALL });

        return NriBuffer;
    }

    void Device::Release(
        nri::Buffer& NriBuffer,
        bool         Defer)
    {
        m_Impl->EndTracking(&NriBuffer);
        m_Impl->Release(NriBuffer, Defer);
    }

    void DeviceImpl::Release(
        nri::Buffer& NriBuffer,
        bool         Defer)
    {
        if (Defer)
        {
            m_FrameManager.DeferRelease(NriBuffer);
        }
        else
        {
            m_MemoryAllocator.Release(&NriBuffer);
        }
    }

    //

    void DeviceImpl::BeginTracking(
        nri::Buffer*     Buffer,
        nri::AccessStage InitialState)
    {
        m_ResourceStateTracker.BeginTracking(Buffer, InitialState);
    }

    void DeviceImpl::EndTracking(
        nri::Buffer* Buffer)
    {
        m_ResourceStateTracker.EndTracking(Buffer);
    }
} // namespace Ame::Rhi