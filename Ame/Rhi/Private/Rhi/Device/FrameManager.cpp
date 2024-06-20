#include <Rhi/Device/FrameManager.hpp>
#include <Rhi/Device/Wrapper/DeviceWrapper.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    void FrameManager::Initialize(
        IDeviceWrapper& deviceWrapper,
        uint8_t         framesInFlightCount)
    {
        auto& nri       = deviceWrapper.GetNri();
        auto& nriDevice = deviceWrapper.GetNriDevice();
        auto& nriCore   = *nri.GetCoreInterface();

        m_FramesInFlightCount = framesInFlightCount;
        ThrowIfFailed(nriCore.CreateFence(nriDevice, m_FenceValue, m_Fence), "Failed to create a frame fence");
        m_Frames = std::make_unique<Frame[]>(framesInFlightCount);
    }

    void FrameManager::Shutdown(
        nri::CoreInterface& nriCore)
    {
        for (uint8_t i = 0; i < m_FramesInFlightCount; i++)
        {
            m_Frames[i].Shutdown(nriCore);
        }
        nriCore.DestroyFence(*m_Fence);
        m_Fence = nullptr;
    }

    //

    void FrameManager::NewFrame(
        nri::CoreInterface&     nriCore,
        IDeviceMemoryAllocator& memoryAllocator)
    {
        if (m_FenceValue >= m_FramesInFlightCount) [[likely]]
        {
            nriCore.Wait(*m_Fence, 1 + m_FenceValue - m_FramesInFlightCount);
        }

        auto& frame = GetCurrentFrame();
        frame.NewFrame(nriCore, memoryAllocator);
    }

    void FrameManager::AdvanceFrame(
        nri::CoreInterface& nriCore,
        nri::CommandQueue&  graphicsQueue)
    {
        nri::FenceSubmitDesc fenceDesc{
            .fence = m_Fence,
            .value = ++m_FenceValue
        };

        nri::QueueSubmitDesc submitDesc{
            .signalFences   = &fenceDesc,
            .signalFenceNum = 1,
        };
        nriCore.QueueSubmit(graphicsQueue, submitDesc);
    }

    void FrameManager::FlushIdle(
        nri::CoreInterface&     nriCore,
        IDeviceMemoryAllocator& memoryAllocator)
    {
        for (uint32_t i = 0; i < m_FramesInFlightCount; ++i)
        {
            auto& frame = m_Frames[i];
            frame.Release(nriCore, memoryAllocator);
        }
    }

    //

    void FrameManager::DeferRelease(
        nri::Buffer& nriBuffer)
    {
        auto& frame = GetCurrentFrame();
        frame.DeferRelease(nriBuffer);
    }

    void FrameManager::DeferRelease(
        nri::Texture& nriTexture)
    {
        auto& frame = GetCurrentFrame();
        frame.DeferRelease(nriTexture);
    }

    void FrameManager::DeferRelease(
        nri::Descriptor& nriDescriptor)
    {
        auto& frame = GetCurrentFrame();
        frame.DeferRelease(nriDescriptor);
    }

    void FrameManager::DeferRelease(
        nri::Pipeline& nriPipeline)
    {
        auto& frame = GetCurrentFrame();
        frame.DeferRelease(nriPipeline);
    }

    Rhi::Frame& FrameManager::GetCurrentFrame() const noexcept
    {
        return m_Frames[GetFrameIndex()];
    }

    //

    uint64_t FrameManager::GetFrameCount() const
    {
        return m_FenceValue;
    }

    uint8_t FrameManager::GetFrameIndex() const
    {
        return static_cast<uint8_t>(GetFrameCount() % GetFrameCountInFlight());
    }

    uint8_t FrameManager::GetFrameCountInFlight() const
    {
        return m_FramesInFlightCount;
    }
} // namespace Ame::Rhi