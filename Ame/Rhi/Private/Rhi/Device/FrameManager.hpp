#pragma once

#include <Core/Ame.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/FrameWrapper.hpp>

namespace Ame::Rhi
{
    class NRIBridge;

    class FrameManager : public NonCopyable, public NonMovable
    {
    public:
        void Initialize(
            nri::CoreInterface& NriCore,
            nri::Device&        RhiDevice,
            nri::CommandQueue&  CommandQueue,
            uint32_t            FramesInFlightCount);

        void Shutdown(
            nri::CoreInterface& NriCore);

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

        /// <summary>
        /// Get the current command list.
        /// </summary>
        [[nodiscard]] CommandListImpl& GetCurrentCommandList() const noexcept;

    public:
        /// <summary>
        /// Mark the start of frame.
        /// Wait for previous frame to finish.
        /// </summary>
        void NewFrame(
            nri::CoreInterface& NriCore,
            MemoryAllocator&    MemAllocator);

        /// <summary>
        /// End the frame.
        /// </summary>
        void EndFrame(
            nri::CoreInterface& NriCore,
            nri::CommandQueue&  GraphicsQueue);

        /// <summary>
        /// Advance to the next frame.
        /// </summary>
        void AdvanceFrame(
            nri::CoreInterface& NriCore,
            nri::CommandQueue&  GraphicsQueue);

        /// <summary>
        /// Flush all idle resources
        /// </summary>
        void FlushIdle(
            nri::CoreInterface& NriCore,
            MemoryAllocator&    MemAllocator);

    public:
        /// <summary>
        /// Defer the release of a buffer.
        /// </summary>
        void DeferRelease(
            nri::Buffer& NriBuffer);

        /// <summary>
        /// Defer the release of a texture.
        /// </summary>
        void DeferRelease(
            nri::Texture& NriTexture);

        /// <summary>
        /// Defer the release of a descriptor.
        /// </summary>
        void DeferRelease(
            nri::Descriptor& NriDescriptor);

        /// <summary>
        /// Defer the release of a pipeline state.
        /// </summary>
        void DeferRelease(
            nri::Pipeline& Pipeline);

    private:
        /// <summary>
        /// Get the current frame.
        /// </summary>
        [[nodiscard]] auto& GetCurrentFrame() const noexcept
        {
            return m_FrameWrapper.Frames[GetFrameIndex()];
        }

    private:
        FrameWrapper m_FrameWrapper;
    };
} // namespace Ame::Rhi