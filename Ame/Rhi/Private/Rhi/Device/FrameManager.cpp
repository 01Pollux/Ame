#include <Rhi/Device/FrameManager.hpp>

namespace Ame::Rhi
{
    void FrameManager::Initialize(
        DeviceImpl&                     rhiDevice,
        const DescriptorAllocationDesc& descriptorPoolDesc,
        uint32_t                        framesInFlightCount)
    {
        m_FrameWrapper.Initialize(rhiDevice, descriptorPoolDesc, framesInFlightCount);
    }

    void FrameManager::Shutdown(
        nri::CoreInterface& nriCore)
    {
        m_FrameWrapper.Shutdown(nriCore);
    }

    //

    void FrameManager::NewFrame(
        nri::CoreInterface& nriCore,
        MemoryAllocator&    memoryAllocator)
    {
        m_FrameWrapper.Sync(nriCore);
        m_FrameWrapper.NewFrame(nriCore, memoryAllocator, GetFrameIndex());
    }

    void FrameManager::EndFrame()
    {
        m_FrameWrapper.EndFrame(GetFrameIndex());
    }

    void FrameManager::AdvanceFrame(
        nri::CoreInterface& nriCore,
        nri::CommandQueue&  graphicsQueue)
    {
        m_FrameWrapper.AdvanceFrame(nriCore, graphicsQueue);
    }

    void FrameManager::FlushIdle(
        nri::CoreInterface& nriCore,
        MemoryAllocator&    memoryAllocator)
    {
        for (uint32_t i = 0; i < m_FrameWrapper.FramesInFlightCount; ++i)
        {
            m_FrameWrapper.Release(nriCore, memoryAllocator, i);
        }
    }

    //

    void FrameManager::DeferRelease(
        nri::Buffer& nriBuffer)
    {
        GetCurrentFrame().DeferRelease(nriBuffer);
    }

    void FrameManager::DeferRelease(
        nri::Texture& nriTexture)
    {
        GetCurrentFrame().DeferRelease(nriTexture);
    }

    void FrameManager::DeferRelease(
        nri::Descriptor& nriDescriptor)
    {
        GetCurrentFrame().DeferRelease(nriDescriptor);
    }

    void FrameManager::DeferRelease(
        nri::Pipeline& nriPipeline)
    {
        GetCurrentFrame().DeferRelease(nriPipeline);
    }

    Rhi::Frame& FrameManager::GetCurrentFrame() const noexcept
    {
        return m_FrameWrapper.Frames[GetFrameIndex()];
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

    CommandListImpl& FrameManager::GetCurrentCommandList() const noexcept
    {
        return GetCurrentFrame().GetCommandList();
    }
} // namespace Ame::Rhi