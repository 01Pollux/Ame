#pragma once

#include <Core/Ame.hpp>
#include <Rhi/Core.hpp>

#include <Rhi/Device/Frame.hpp>

namespace Ame::Rhi
{
    struct FrameWrapper
    {
        nri::Fence*   Fence = nullptr;
        UPtr<Frame[]> Frames;
        uint64_t      FenceValue          = 0;
        uint32_t      FramesInFlightCount = 0;

        /// <summary>
        /// Initialize the frame wrapper.
        /// </summary>
        void Initialize(
            DeviceImpl&                     rhiDevice,
            const DescriptorAllocationDesc& descriptorPoolDesc,
            uint32_t                        framesInFlightCount);

        /// <summary>
        /// Shutdown the frame wrapper.
        /// </summary>
        void Shutdown(
            nri::CoreInterface& nriCore);

        /// <summary>
        /// Sync the swapchain.
        /// </summary>
        void Sync(
            nri::CoreInterface& nriCore);

        /// <summary>
        /// Start a new frame and cleanup pending resources.
        /// </summary>
        void NewFrame(
            nri::CoreInterface& nriCore,
            MemoryAllocator&    memAllocator,
            uint32_t            frameIndex);

        /// <summary>
        /// End the frame.
        /// </summary>
        void EndFrame(
            uint32_t frameIndex);

        /// <summary>
        /// Advance to the next frame.
        /// </summary>
        void AdvanceFrame(
            nri::CoreInterface& nriCore,
            nri::CommandQueue&  graphicsQueue);

        /// <summary>
        /// Release the frame and all deferred resources.
        /// </summary>
        void Release(
            nri::CoreInterface& nriCore,
            MemoryAllocator&    memAllocator,
            uint32_t            frameIndex);
    };
} // namespace Ame::Rhi