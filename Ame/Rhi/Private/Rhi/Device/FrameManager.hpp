#pragma once

#include <Rhi/Device.hpp>
#include "StateTracker.hpp"

namespace Ame::Rhi
{
    class NRIBridge;

    class FrameManager : public NonCopyable, public NonMovable
    {
    public:
        FrameManager(
            NRIBridge& NriBridge);

    public:
        /// <summary>
        /// Get the number of frames that have been rendered.
        /// This is the number of frames that have been presented.
        /// </summary>
        [[nodiscard]] uint64_t GetFrameCount() const;

        /// <summary>
        /// Get the index of the current frame.
        /// This is the index of the frame that is currently being rendered.
        /// </summary>
        [[nodiscard]] uint8_t GetFrameIndex() const;

        /// <summary>
        /// Get the number of frames that are in flight.
        /// </summary>
        [[nodiscard]] uint8_t GetFrameCountInFlight() const;

    public:
        /// <summary>
        /// Mark the start of frame.
        /// Wait for previous frame to finish.
        /// </summary>
        void NewFrame();

        /// <summary>
        /// Mark the end of frame.
        /// </summary>
        void EndFrame(
            nri::CommandQueue& GraphicsQueue);

        /// <summary>
        /// Flush all idle resources
        /// </summary>
        void FlushIdle();

    private:
        ResourceStateTracker m_ResourceStateTracker;
    };
} // namespace Ame::Rhi