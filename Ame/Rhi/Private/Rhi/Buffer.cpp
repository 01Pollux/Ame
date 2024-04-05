#include <Rhi/Buffer.hpp>
#include <Rhi/Device.hpp>

#include "NriError.hpp"

namespace Ame::Rhi
{
    Buffer::Buffer(
        Device&           RhiDevice,
        const BufferDesc& Desc) :
        m_Buffer(RhiDevice.Create(Desc))
    {
    }

    //

    void Buffer::Release(
        Device& RhiDevice)
    {
        RhiDevice.Release(*m_Buffer, false);
        m_Buffer = nullptr;
    }

    void Buffer::DeferRelease(
        Device& RhiDevice)
    {
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
} // namespace Ame::Rhi