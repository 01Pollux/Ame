#pragma once

#include <Core/Ame.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/FrameWrapper.hpp>

namespace Ame::Rhi
{
    class NRIBridge;

    class FrameManager
    {
    public:
        FrameManager() = default;

        FrameManager(const FrameManager&)            = delete;
        FrameManager& operator=(const FrameManager&) = delete;

        FrameManager(FrameManager&&)            = delete;
        FrameManager& operator=(FrameManager&&) = delete;

        ~FrameManager() = default;

    public:
        void Initialize(
            DeviceImpl&                     rhiDevice,
            const DescriptorAllocationDesc& descriptorPoolDesc,
            uint32_t                        framesInFlightCount);

        void Shutdown(
            nri::CoreInterface& nriCore);

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
            nri::CoreInterface& nriCore,
            MemoryAllocator&    memoryAllocator);

        /// <summary>
        /// End the frame.
        /// </summary>
        void EndFrame();

        /// <summary>
        /// Advance to the next frame.
        /// </summary>
        void AdvanceFrame(
            nri::CoreInterface& nriCore,
            nri::CommandQueue&  graphicsQueue);

        /// <summary>
        /// Flush all idle resources
        /// </summary>
        void FlushIdle(
            nri::CoreInterface& nriCore,
            MemoryAllocator&    memoryAllocator);

    public:
        /// <summary>
        /// Defer the release of a buffer.
        /// </summary>
        void DeferRelease(
            nri::Buffer& nriBuffer);

        /// <summary>
        /// Defer the release of a texture.
        /// </summary>
        void DeferRelease(
            nri::Texture& nriTexture);

        /// <summary>
        /// Defer the release of a descriptor.
        /// </summary>
        void DeferRelease(
            nri::Descriptor& nriDescriptor);

        /// <summary>
        /// Defer the release of a pipeline state.
        /// </summary>
        void DeferRelease(
            nri::Pipeline& nriPipeline);

    private:
        /// <summary>
        /// Get the current frame.
        /// </summary>
        [[nodiscard]] Rhi::Frame& GetCurrentFrame() const noexcept;

    private:
        FrameWrapper m_FrameWrapper;
    };
} // namespace Ame::Rhi