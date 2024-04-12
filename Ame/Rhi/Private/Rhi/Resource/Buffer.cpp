#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    Buffer::Buffer(
        Device&           RhiDevice,
        MemoryLocation    Location,
        const BufferDesc& Desc) :
        m_Buffer(RhiDevice.Create(Location, Desc))
    {
    }

    //

    void Buffer::Release(
        Device& RhiDevice)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_Buffer != nullptr, "Buffer was already released.");
        RhiDevice.Release(*m_Buffer, false);
        m_Buffer = nullptr;
    }

    void Buffer::DeferRelease(
        Device& RhiDevice)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_Buffer != nullptr, "Buffer was already released.");
        RhiDevice.Release(*m_Buffer, true);
        m_Buffer = nullptr;
    }

    //

    void Buffer::SetName(
        Device&     RhiDevice,
        const char* Name)
    {
        RhiDevice.SetName(*m_Buffer, Name);
    }

    const BufferDesc& Buffer::GetDesc(
        Device& RhiDevice) const
    {
        return RhiDevice.GetDesc(*m_Buffer);
    }

    nri::Buffer* Buffer::Unwrap(
        Device& RhiDevice) const
    {
        return m_Buffer;
    }

    void* Buffer::GetNative(
        Device& RhiDevice) const
    {
        return RhiDevice.GetNative(*m_Buffer);
    }

    //

    nri::Buffer* Device::Create(
        MemoryLocation    Location,
        const BufferDesc& Desc)
    {
        auto& Nri     = m_Impl->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        auto NriBuffer = m_Impl->m_MemoryAllocator.CreateBuffer(*m_Impl, Location, Desc);
        m_Impl->BeginTracking(NriBuffer, { nri::AccessBits::UNKNOWN, nri::StageBits::ALL });

        return NriBuffer;
    }

    void Device::Release(
        nri::Buffer& NriBuffer,
        bool         Defer)
    {
        m_Impl->EndTracking(&NriBuffer);
        if (Defer)
        {
            auto& Nri     = m_Impl->GetNRI();
            auto& NriCore = *Nri.GetCoreInterface();

            NriCore.DestroyBuffer(NriBuffer);
        }
        else
        {
            m_Impl->DeferRelease(NriBuffer);
        }
    }

    void Device::SetName(
        nri::Buffer& NriBuffer,
        const char*  Name)
    {
        auto& Nri     = m_Impl->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.SetBufferDebugName(NriBuffer, Name);
    }

    const BufferDesc& Device::GetDesc(
        nri::Buffer& NriBuffer) const
    {
        auto& Nri     = m_Impl->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        return NriCore.GetBufferDesc(NriBuffer);
    }

    void* Device::GetNative(
        nri::Buffer& NriBuffer) const
    {
        auto& Nri = m_Impl->GetNRI();
        return std::bit_cast<void*>(Nri.GetCoreInterface()->GetBufferNativeObject(NriBuffer));
    }
} // namespace Ame::Rhi