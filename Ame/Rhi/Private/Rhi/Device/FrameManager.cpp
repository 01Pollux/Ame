#include "FrameManager.hpp"
#include "../Nri/Bridge.hpp"

namespace Ame::Rhi
{
    FrameManager::FrameManager(
        NRIBridge& NriBridge) :
        m_ResourceStateTracker(*NriBridge.GetCoreInterface())
    {
    }

    //

    void FrameManager::NewFrame()
    {
    }

    void FrameManager::EndFrame(
        nri::CommandQueue& GraphicsQueue)
    {
    }

    void FrameManager::FlushIdle()
    {
    }

    //

    void FrameManager::DeferRelease(
        nri::Buffer& NriBuffer)
    {
    }

    void FrameManager::DeferRelease(
        nri::Texture& NriTexture)
    {
    }

    void FrameManager::DeferRelease(
        nri::Descriptor& NriDescriptor)
    {
    }

    //

    uint64_t FrameManager::GetFrameCount() const
    {
        return 0;
    }

    uint8_t FrameManager::GetFrameIndex() const
    {
        return static_cast<uint8_t>(GetFrameCount() % GetFrameCountInFlight());
    }

    uint8_t FrameManager::GetFrameCountInFlight() const
    {
        return 0;
    }
} // namespace Ame::Rhi