#include "FrameManager.hpp"

#include "../NriError.hpp"

namespace Ame::Rhi
{
    void FrameWrapper::Initialize(
        nri::CoreInterface& NriCore,
        nri::Device&        RhiDevice,
        uint32_t            FramesInFlightCount)
    {
        Frames                    = std::make_unique<Frame[]>(FramesInFlightCount);
        this->FramesInFlightCount = FramesInFlightCount;
        ThrowIfFailed(NriCore.CreateFence(RhiDevice, FenceValue, Fence), "Failed to create a frame fence");
    }

    void FrameWrapper::Shutdown(
        nri::CoreInterface& NriCore)
    {
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
        uint32_t FrameIndex)
    {
        auto& Frame = Frames[FrameIndex];
    }

    void FrameWrapper::EndFrame(
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
        uint32_t                 FrameIndex)
    {
        auto& Frame = Frames[FrameIndex];
    }
} // namespace Ame::Rhi