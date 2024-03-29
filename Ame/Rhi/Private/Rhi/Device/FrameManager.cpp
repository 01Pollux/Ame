#include "FrameManager.hpp"
#include "../Nri/Bridge.hpp"

namespace Ame::Rhi
{
    void FrameManager::Initialize(
        nri::CoreInterface& NriCore,
        nri::Device&        RhiDevice,
        uint32_t            FramesInFlightCount) 
    {
        m_FrameWrapper.Initialize(NriCore, RhiDevice, FramesInFlightCount);
    }

    void FrameManager::Shutdown(
        nri::CoreInterface& NriCore)
    {
        m_FrameWrapper.Shutdown(NriCore);
    }

    //

    void FrameManager::NewFrame(
        nri::CoreInterface& NriCore)
    {
        m_FrameWrapper.Sync(NriCore);
        m_FrameWrapper.NewFrame(GetFrameIndex());
    }

    void FrameManager::EndFrame(
        nri::CoreInterface& NriCore,
        nri::CommandQueue&  GraphicsQueue)
    {
        m_FrameWrapper.EndFrame(NriCore, GraphicsQueue);
    }

    void FrameManager::FlushIdle()
    {
        for (uint32_t i = 0; i < m_FrameWrapper.FramesInFlightCount; ++i)
        {
            m_FrameWrapper.Release(i);
        }
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
        return m_FrameWrapper.FenceValue;
    }

    uint8_t FrameManager::GetFrameIndex() const
    {
        return static_cast<uint8_t>(GetFrameCount() % GetFrameCountInFlight());
    }

    uint8_t FrameManager::GetFrameCountInFlight() const
    {
        return m_FrameWrapper.FramesInFlightCount;
    }
} // namespace Ame::Rhi