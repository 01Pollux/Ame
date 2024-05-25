#include <Rhi/Device/FrameWrapper.hpp>
#include <Rhi/Device/DeviceImpl.hpp>
#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    void FrameWrapper::Initialize(
        DeviceImpl&                     rhiDevice,
        const DescriptorAllocationDesc& descriptorPoolDesc,
        uint32_t                        framesInFlightCount)
    {
        auto& nriUtils = rhiDevice.GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        Frames = std::make_unique<Frame[]>(framesInFlightCount);
        for (uint32_t i = 0; i < framesInFlightCount; i++)
        {
            Frames[i].Initialize(rhiDevice, descriptorPoolDesc, i);
        }

        this->FramesInFlightCount = framesInFlightCount;
        ThrowIfFailed(nriCore.CreateFence(rhiDevice.GetDevice(), FenceValue, Fence), "Failed to create a frame fence");
    }

    void FrameWrapper::Shutdown(
        nri::CoreInterface& nriCore)
    {
        for (uint32_t i = 0; i < FramesInFlightCount; i++)
        {
            Frames[i].Shutdown();
        }
        nriCore.DestroyFence(*Fence);
        Fence = nullptr;
    }

    void FrameWrapper::Sync(
        nri::CoreInterface& nriCore)
    {
        if (FenceValue >= FramesInFlightCount) [[likely]]
        {
            nriCore.Wait(*Fence, 1 + FenceValue - FramesInFlightCount);
        }
    }

    //

    void FrameWrapper::NewFrame(
        nri::CoreInterface& nriCore,
        MemoryAllocator&    memoryAllocator,
        uint32_t            frameIndex)
    {
        auto& frame = Frames[frameIndex];
        frame.NewFrame(nriCore, memoryAllocator);
    }

    void FrameWrapper::EndFrame(
        uint32_t frameIndex)
    {
        auto& frame       = Frames[frameIndex];
        auto& commandList = frame.GetCommandList();

        frame.EndFrame();
        commandList.Submit();
    }

    void FrameWrapper::AdvanceFrame(
        nri::CoreInterface& nriCore,
        nri::CommandQueue&  graphicsQueue)
    {
        nri::FenceSubmitDesc fenceDesc{
            .fence = Fence,
            .value = ++FenceValue
        };

        nri::QueueSubmitDesc submitDesc{
            .signalFences   = &fenceDesc,
            .signalFenceNum = 1,
        };
        nriCore.QueueSubmit(graphicsQueue, submitDesc);
    }

    void FrameWrapper::Release(
        nri::CoreInterface& nriCore,
        MemoryAllocator&    memoryAllocator,
        uint32_t            frameIndex)
    {
        auto& frame = Frames[frameIndex];
        frame.Release(nriCore, memoryAllocator);
    }
} // namespace Ame::Rhi