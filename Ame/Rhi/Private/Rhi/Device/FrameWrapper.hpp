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
            DeviceImpl&                     RhiDevice,
            const DescriptorAllocationDesc& DescriptorPoolDesc,
            uint32_t                        FramesInFlightCount);

        /// <summary>
        /// Shutdown the frame wrapper.
        /// </summary>
        void Shutdown(
            nri::CoreInterface& NriCore);

        /// <summary>
        /// Sync the swapchain.
        /// </summary>
        /// <param name="GraphicsQueue"></param>
        void Sync(
            nri::CoreInterface& NriCore);

        /// <summary>
        /// Start a new frame and cleanup pending resources.
        /// </summary>
        void NewFrame(
            nri::CoreInterface& NriCore,
            MemoryAllocator&    MemAllocator,
            uint32_t            FrameIndex);

        /// <summary>
        /// End the frame.
        /// </summary>
        void EndFrame(
            uint32_t FrameIndex);

        /// <summary>
        /// Advance to the next frame.
        /// </summary>
        void AdvanceFrame(
            nri::CoreInterface& NriCore,
            nri::CommandQueue&  GraphicsQueue);

        /// <summary>
        /// Release the frame and all deferred resources.
        /// </summary>
        void Release(
            nri::CoreInterface& NriCore,
            MemoryAllocator&    MemAllocator,
            uint32_t            FrameIndex);
    };
} // namespace Ame::Rhi