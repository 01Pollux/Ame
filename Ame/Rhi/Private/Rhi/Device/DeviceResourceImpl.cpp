#include "DeviceImpl.hpp"
#include "FrameManager.hpp"

namespace Ame::Rhi
{
    void Device::Impl::BeginTracking(
        nri::Buffer*     Buffer,
        nri::AccessStage InitialState)
    {
        m_ResourceStateTracker.BeginTracking(Buffer, InitialState);
    }

    void Device::Impl::BeginTracking(
        nri::Texture*          Texture,
        nri::AccessLayoutStage InitialState)
    {
        m_ResourceStateTracker.BeginTracking(*m_NRI.GetCoreInterface(), Texture, InitialState);
    }

    void Device::Impl::EndTracking(
        nri::Buffer* Buffer)
    {
        m_ResourceStateTracker.EndTracking(Buffer);
    }

    void Device::Impl::EndTracking(
        nri::Texture* Texture)
    {
        m_ResourceStateTracker.EndTracking(Texture);
    }

    //

    void Device::Impl::DeferRelease(
        nri::Buffer& NriBuffer)
    {
        m_FrameManager.DeferRelease(NriBuffer);
    }

    void Device::Impl::DeferRelease(
        nri::Texture& NriTexture)
    {
        m_FrameManager.DeferRelease(NriTexture);
    }

    void Device::Impl::DeferRelease(
        nri::Descriptor& NriDescriptor)
    {
        m_FrameManager.DeferRelease(NriDescriptor);
    }
} // namespace Ame::Rhi