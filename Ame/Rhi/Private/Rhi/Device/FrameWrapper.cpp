#include "FrameWrapper.hpp"

#include "../NriError.hpp"

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
            Frames[i].Initialize(NriCore, GraphicsQueue);
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
        uint32_t            FrameIndex)
    {
        auto& CurFrame = Frames[FrameIndex];
        CurFrame.NewFrame(NriCore);
    }

    void FrameWrapper::EndFrame(
        nri::CoreInterface& NriCore,
        nri::CommandQueue&  GraphicsQueue,
        uint32_t            FrameIndex)
    {
        auto& CurFrame      = Frames[FrameIndex];
        auto  CommandBuffer = CurFrame.GetCommandList();

        CurFrame.EndFrame(NriCore);

        nri::QueueSubmitDesc SubmitDesc{
            .commandBuffers   = &CommandBuffer,
            .commandBufferNum = 1
        };
        NriCore.QueueSubmit(GraphicsQueue, SubmitDesc);
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
            .signalFences     = &FenceDesc,
            .signalFenceNum   = 1,
        };
        NriCore.QueueSubmit(GraphicsQueue, SubmitDesc);
    }

    void FrameWrapper::Release(
        uint32_t FrameIndex)
    {
        auto& CurFrame = Frames[FrameIndex];
    }
} // namespace Ame::Rhi