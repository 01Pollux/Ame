#include "DeviceImpl.hpp"
#include "FrameManager.hpp"

namespace Ame::Rhi
{
    void Device::Impl::BeginTracking(
        nri::Buffer*     Buffer,
        nri::AccessStage InitialState)
    {
        auto& Tracker = m_FrameManager.GetStateTracker();
        Tracker.BeginTracking(Buffer, InitialState);
    }

    void Device::Impl::BeginTracking(
        nri::Texture*          Texture,
        nri::AccessLayoutStage InitialState)
    {
        auto& Tracker = m_FrameManager.GetStateTracker();
        Tracker.BeginTracking(*m_NRI.GetCoreInterface(), Texture, InitialState);
    }

    void Device::Impl::EndTracking(
        nri::Buffer* Buffer)
    {
        auto& Tracker = m_FrameManager.GetStateTracker();
        Tracker.EndTracking(Buffer);
    }

    void Device::Impl::EndTracking(
        nri::Texture* Texture)
    {
        auto& Tracker = m_FrameManager.GetStateTracker();
        Tracker.EndTracking(Texture);
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