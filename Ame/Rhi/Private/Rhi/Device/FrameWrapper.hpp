#pragma once

#include <NRI.h>
#include <Rhi/Core.hpp>
#include "Frame.hpp"

namespace Ame::Rhi
{
    struct FrameWrapper : public NonCopyable,
                          public NonMovable
    {
        nri::Fence*   Fence = nullptr;
        UPtr<Frame[]> Frames;
        uint64_t      FenceValue          = 0;
        uint32_t      FramesInFlightCount = 0;

        /// <summary>
        /// Initialize the frame wrapper.
        /// </summary>
        void Initialize(
            nri::CoreInterface& NriCore,
            nri::Device&        RhiDevice,
            uint32_t            FramesInFlightCount);

        /// <summary>
        /// Shutdown the frame wrapper.
        /// </summary>
        void Shutdown(
            nri::CoreInterface& NriCore);

        void Sync(
            nri::CoreInterface& NriCore);

        void NewFrame(
            uint32_t FrameIndex);

        void EndFrame(
            nri::CoreInterface& NriCore,
            nri::CommandQueue&  GraphicsQueue);

        void Release(
            uint32_t FrameIndex);
    };
} // namespace Ame::Rhi