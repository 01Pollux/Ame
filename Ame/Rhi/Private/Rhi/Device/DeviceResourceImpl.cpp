#include "DeviceImpl.hpp"
#include "FrameManager.hpp"

namespace Ame::Rhi
{
    void DeviceImpl::BeginTracking(
        nri::Buffer*     Buffer,
        nri::AccessStage InitialState)
    {
        m_ResourceStateTracker.BeginTracking(Buffer, InitialState);
    }

    void DeviceImpl::BeginTracking(
        nri::Texture*          Texture,
        nri::AccessLayoutStage InitialState)
    {
        m_ResourceStateTracker.BeginTracking(*m_NRI.GetCoreInterface(), Texture, InitialState);
    }

    void DeviceImpl::EndTracking(
        nri::Buffer* Buffer)
    {
        m_ResourceStateTracker.EndTracking(Buffer);
    }

    void DeviceImpl::EndTracking(
        nri::Texture* Texture)
    {
        m_ResourceStateTracker.EndTracking(Texture);
    }

    //

    void DeviceImpl::DeferRelease(
        nri::Buffer& NriBuffer)
    {
        m_FrameManager.DeferRelease(NriBuffer);
    }

    void DeviceImpl::DeferRelease(
        nri::Texture& NriTexture)
    {
        m_FrameManager.DeferRelease(NriTexture);
    }

    void DeviceImpl::DeferRelease(
        nri::Descriptor& NriDescriptor)
    {
        m_FrameManager.DeferRelease(NriDescriptor);
    }
} // namespace Ame::Rhi