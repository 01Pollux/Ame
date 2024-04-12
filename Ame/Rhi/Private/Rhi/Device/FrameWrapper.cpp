#include <Rhi/Device/FrameWrapper.hpp>
#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    void FrameWrapper::Initialize(
        nri::CoreInterface& NriCore,
        nri::Device&        RhiDevice,
        nri::CommandQueue&  GraphicsQueue,
        uint32_t            FramesInFlightCount)
    {
        Frames = std::make_unique<Frame[]>(FramesInFlightCount);
        for (uint32_t i = 0; i < FramesInFlightCount; i++)
        {
            Frames[i].Initialize(NriCore, GraphicsQueue, i);
        }

        this->FramesInFlightCount = FramesInFlightCount;
        ThrowIfFailed(NriCore.CreateFence(RhiDevice, FenceValue, Fence), "Failed to create a frame fence");
    }

    void FrameWrapper::Shutdown(
        nri::CoreInterface& NriCore)
    {
        for (uint32_t i = 0; i < FramesInFlightCount; i++)
        {
            Frames[i].Shutdown(NriCore);
        }
        NriCore.DestroyFence(*Fence);
        Fence = nullptr;
    }

    void FrameWrapper::Sync(
        nri::CoreInterface& NriCore)
    {
        if (FenceValue >= FramesInFlightCount) [[likely]]
        {
            NriCore.Wait(*Fence, 1 + FenceValue - FramesInFlightCount);
        }
    }

    //

    void FrameWrapper::NewFrame(
        nri::CoreInterface& NriCore,
        MemoryAllocator&    MemAllocator,
        uint32_t            FrameIndex)
    {
        auto& CurFrame = Frames[FrameIndex];
        CurFrame.NewFrame(NriCore, MemAllocator);
    }

    void FrameWrapper::EndFrame(
        nri::CoreInterface& NriCore,
        nri::CommandQueue&  GraphicsQueue,
        uint32_t            FrameIndex)
    {
        auto& CurFrame = Frames[FrameIndex];
        auto& CmdList  = CurFrame.GetCommandList();

        CurFrame.EndFrame(NriCore);
        CmdList.Submit(NriCore, GraphicsQueue);
    }

    void FrameWrapper::AdvanceFrame(
        nri::CoreInterface& NriCore,
        nri::CommandQueue&  GraphicsQueue)
    {
        nri::FenceSubmitDesc FenceDesc{
            .fence = Fence,
            .value = ++FenceValue
        };

        nri::QueueSubmitDesc SubmitDesc{
            .signalFences   = &FenceDesc,
            .signalFenceNum = 1,
        };
        NriCore.QueueSubmit(GraphicsQueue, SubmitDesc);
    }

    void FrameWrapper::Release(
        nri::CoreInterface& NriCore,
        MemoryAllocator&    MemAllocator,
        uint32_t            FrameIndex)
    {
        auto& CurFrame = Frames[FrameIndex];
        CurFrame.Release(NriCore, MemAllocator);
    }
} // namespace Ame::Rhi