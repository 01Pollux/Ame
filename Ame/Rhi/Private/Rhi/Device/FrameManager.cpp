#include <Rhi/Device/FrameManager.hpp>

namespace Ame::Rhi
{
    void FrameManager::Initialize(
        DeviceImpl&                     RhiDevice,
        const DescriptorAllocationDesc& DescriptorPoolDesc,
        uint32_t                        FramesInFlightCount)
    {
        m_FrameWrapper.Initialize(RhiDevice, DescriptorPoolDesc, FramesInFlightCount);
    }

    void FrameManager::Shutdown(
        nri::CoreInterface& NriCore)
    {
        m_FrameWrapper.Shutdown(NriCore);
    }

    //

    void FrameManager::NewFrame(
        nri::CoreInterface& NriCore,
        MemoryAllocator&    MemAllocator)
    {
        m_FrameWrapper.Sync(NriCore);
        m_FrameWrapper.NewFrame(NriCore, MemAllocator, GetFrameIndex());
    }

    void FrameManager::EndFrame()
    {
        m_FrameWrapper.EndFrame(GetFrameIndex());
    }

    void FrameManager::AdvanceFrame(
        nri::CoreInterface& NriCore,
        nri::CommandQueue&  GraphicsQueue)
    {
        m_FrameWrapper.AdvanceFrame(NriCore, GraphicsQueue);
    }

    void FrameManager::FlushIdle(
        nri::CoreInterface& NriCore,
        MemoryAllocator&    MemAllocator)
    {
        for (uint32_t i = 0; i < m_FrameWrapper.FramesInFlightCount; ++i)
        {
            m_FrameWrapper.Release(NriCore, MemAllocator, i);
        }
    }

    //

    void FrameManager::DeferRelease(
        nri::Buffer& NriBuffer)
    {
        GetCurrentFrame().DeferRelease(NriBuffer);
    }

    void FrameManager::DeferRelease(
        nri::Texture& NriTexture)
    {
        GetCurrentFrame().DeferRelease(NriTexture);
    }

    void FrameManager::DeferRelease(
        nri::Descriptor& NriDescriptor)
    {
        GetCurrentFrame().DeferRelease(NriDescriptor);
    }

    void FrameManager::DeferRelease(
        nri::Pipeline& Pipeline)
    {
        GetCurrentFrame().DeferRelease(Pipeline);
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