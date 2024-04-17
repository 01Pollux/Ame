#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    Buffer::Buffer(
        Device&        RhiDevice,
        MemoryLocation Location,
        nri::Buffer*   NriBuffer) :
        m_Device(&RhiDevice.GetImpl()),
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
        Extern,
        DeviceImpl&  RhiDevice,
        nri::Buffer* NriBuffer) :
        m_Device(&RhiDevice),
        m_Buffer(NriBuffer),
        m_Owning(false)
    {
    }

    Buffer::Buffer(
        Extern,
        Device&      RhiDevice,
        nri::Buffer* NriBuffer) :
        m_Device(&RhiDevice.GetImpl()),
        m_Buffer(NriBuffer),
        m_Owning(false)
    {
    }

    Buffer::Buffer(
        Device&           RhiDevice,
        MemoryLocation    Location,
        const BufferDesc& Desc) :
        Buffer(RhiDevice, Location, RhiDevice.Create(Location, Desc))
    {
    }

    Buffer::Buffer(
        Buffer&& Other) noexcept :
        m_Device(std::exchange(Other.m_Device, nullptr)),
        m_Buffer(std::exchange(Other.m_Buffer, nullptr)),
        m_Mapped(std::exchange(Other.m_Mapped, nullptr)),
        m_Owning(std::exchange(Other.m_Owning, false))
    {
    }

    Buffer& Buffer::operator=(
        Buffer&& Other) noexcept
    {
        if (this != &Other)
        {
            Release();

            m_Device = std::exchange(Other.m_Device, nullptr);
            m_Buffer = std::exchange(Other.m_Buffer, nullptr);
            m_Mapped = std::exchange(Other.m_Mapped, nullptr);
            m_Owning = std::exchange(Other.m_Owning, false);
        }

        return *this;
    }

    Buffer::~Buffer()
    {
        Release();
    }

    //

    nri::Buffer* Buffer::Unwrap() const
    {
        return m_Buffer;
    }

    void* Buffer::GetNative() const
    {
        auto& Nri     = m_Device->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        return std::bit_cast<void*>(NriCore.GetBufferNativeObject(*m_Buffer));
    }

    //

    void Buffer::SetName(
        const char* Name)
    {
        auto& Nri     = m_Device->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.SetBufferDebugName(*m_Buffer, Name);
    }

    const BufferDesc& Buffer::GetDesc() const
    {
        auto& Nri     = m_Device->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        return NriCore.GetBufferDesc(*m_Buffer);
    }

    //

    void* Buffer::GetPtr(
        size_t Offset)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_Mapped != nullptr, "Buffer is not host visible.");
        return static_cast<uint8_t*>(m_Mapped) + Offset;
    }

    BufferResourceView Buffer::CreateView(
        const BufferViewDesc& Desc) const
    {
        return BufferResourceView(m_Device, m_Device->CreateView(*m_Buffer, Desc.Transform(*this)));
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

    //

    void Buffer::Release()
    {
        if (!m_Owning || !m_Device)
        {
            return;
        }

        if (m_Mapped)
        {
            auto& Nri     = m_Device->GetNRI();
            auto& NriCore = *Nri.GetCoreInterface();

            NriCore.UnmapBuffer(*m_Buffer);
            m_Mapped = nullptr;
        }

        m_Device->Release(*m_Buffer);
        m_Buffer = nullptr;
        m_Owning = false;
    }

    void Device::Release(
        nri::Buffer& NriBuffer)
    {
        m_Impl->EndTracking(&NriBuffer);
        m_Impl->Release(NriBuffer);
    }

    void DeviceImpl::Release(
        nri::Buffer& NriBuffer)
    {
        m_FrameManager.DeferRelease(NriBuffer);
    }
} // namespace Ame::Rhi