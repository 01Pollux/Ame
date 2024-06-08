#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    Buffer::Buffer(
        Device&        rhiDevice,
        MemoryLocation location,
        nri::Buffer*   nriBuffer) :
        m_Device(&rhiDevice.GetImpl()),
        m_Buffer(nriBuffer)
    {
        if (location == MemoryLocation::HOST_UPLOAD ||
            location == MemoryLocation::HOST_READBACK)
        {
            auto& nriUtils = m_Device->GetNRI();
            auto& nriCore  = *nriUtils.GetCoreInterface();

            m_Mapped = nriCore.MapBuffer(*m_Buffer, 0, nri::WHOLE_SIZE);
        }
    }

    Buffer::Buffer(
        Extern,
        DeviceImpl&  rhiDeviceImpl,
        nri::Buffer* nriBuffer) :
        m_Device(&rhiDeviceImpl),
        m_Buffer(nriBuffer),
        m_Owning(false)
    {
    }

    Buffer::Buffer(
        Extern,
        Device&      rhiDevice,
        nri::Buffer* nriBuffer) :
        m_Device(&rhiDevice.GetImpl()),
        m_Buffer(nriBuffer),
        m_Owning(false)
    {
    }

    Buffer::Buffer(
        Device&           rhiDevice,
        MemoryLocation    location,
        const BufferDesc& desc) :
        Buffer(rhiDevice, location, rhiDevice.Create(location, desc))
    {
    }

    //

    Buffer::Buffer(
        Buffer&& other) noexcept :
        m_Device(std::exchange(other.m_Device, nullptr)),
        m_Buffer(std::exchange(other.m_Buffer, nullptr)),
        m_Mapped(std::exchange(other.m_Mapped, nullptr)),
        m_Owning(std::exchange(other.m_Owning, false))
    {
    }

    Buffer& Buffer::operator=(
        Buffer&& other) noexcept
    {
        if (this != &other)
        {
            Release();

            m_Device = std::exchange(other.m_Device, nullptr);
            m_Buffer = std::exchange(other.m_Buffer, nullptr);
            m_Mapped = std::exchange(other.m_Mapped, nullptr);
            m_Owning = std::exchange(other.m_Owning, false);
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
        auto& nriUtils = m_Device->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        return std::bit_cast<void*>(nriCore.GetBufferNativeObject(*m_Buffer));
    }

    //

    Buffer Buffer::Borrow() const
    {
        Buffer buffer(Extern{}, *m_Device, m_Buffer);
        buffer.m_Mapped = m_Mapped;
        return buffer;
    }

    bool Buffer::IsOwning() const
    {
        return m_Owning;
    }

    //

    void Buffer::SetName(
        const char* name)
    {
        auto& nriUtils = m_Device->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.SetBufferDebugName(*m_Buffer, name);
    }

    const BufferDesc& Buffer::GetDesc() const
    {
        auto& nriUtils = m_Device->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        return nriCore.GetBufferDesc(*m_Buffer);
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

    BufferResourceView Buffer::CreateView(
        const BufferViewDesc& desc) const
    {
        return BufferResourceView(m_Device, m_Device->CreateView(*m_Buffer, desc.Transform(*this)));
    }

    //

    nri::Buffer* Device::Create(
        MemoryLocation    location,
        const BufferDesc& desc)
    {
        auto& nriUtils = m_Impl->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        auto nriBuffer = m_Impl->m_MemoryAllocator.CreateBuffer(location, desc);
        m_Impl->BeginTracking(nriBuffer, { nri::AccessBits::UNKNOWN, nri::StageBits::ALL });

        return nriBuffer;
    }

    //

    void DeviceImpl::BeginTracking(
        nri::Buffer*     nriBuffer,
        nri::AccessStage initialState)
    {
        m_ResourceStateTracker.BeginTracking(nriBuffer, initialState);
    }

    void DeviceImpl::EndTracking(
        nri::Buffer* nriBuffer)
    {
        m_ResourceStateTracker.EndTracking(nriBuffer);
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
            auto& nriUtils = m_Device->GetNRI();
            auto& nriCore  = *nriUtils.GetCoreInterface();

            nriCore.UnmapBuffer(*m_Buffer);
            m_Mapped = nullptr;
        }

        m_Device->Release(*m_Buffer);
        m_Buffer = nullptr;
        m_Owning = false;
    }

    void Device::Release(
        nri::Buffer& nriBuffer)
    {
        m_Impl->EndTracking(&nriBuffer);
        m_Impl->Release(nriBuffer);
    }

    void DeviceImpl::Release(
        nri::Buffer& nriBuffer)
    {
        EndTracking(&nriBuffer);
        m_FrameManager.DeferRelease(nriBuffer);
    }
} // namespace Ame::Rhi