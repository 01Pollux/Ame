#pragma once

#include <Core/Coroutine.hpp>

#include <Rhi/Descs/Core.hpp>
#include <Rhi/Resource/Backbuffer.hpp>

namespace Ame::Windowing
{
    class Window;
} // namespace Ame::Windowing

namespace Ame::Rhi
{
    struct DeviceCreateDesc;

    class Device
    {
        friend Buffer;
        friend Texture;
        friend ResourceView;
        friend DescriptorSet;
        friend PipelineLayout;
        friend PipelineState;
        friend CommandList;

    public:
        static constexpr bool EnableDrawParametersEmulation = true;

        Device();
        explicit Device(
            const DeviceCreateDesc& desc);

        Device(const Device&)            = delete;
        Device& operator=(const Device&) = delete;

        Device(Device&&);
        Device& operator=(Device&&);

        ~Device();

    public:
        /// <summary>
        /// Check for any leaked rhi objects.
        /// </summary>
        static void CleanupCheck();

    public:
        /// <summary>
        /// Get the graphics API used by the device.
        /// </summary>
        [[nodiscard]] GraphicsAPI GetGraphicsAPI() const;

        /// <summary>
        /// Get the graphics API used by the device.
        /// </summary>
        [[nodiscard]] const char* GetGraphicsAPIName() const;

        /// <summary>
        /// Get the device description.
        /// </summary>
        [[nodiscard]] const DeviceDesc& GetDesc() const;

        /// <summary>
        /// Check if the device is headless.
        /// This means that the device is not rendering to a window.
        /// </summary>
        [[nodiscard]] bool IsHeadless() const;

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
        /// Helper function to get the size of draw indexed command size for indirect execution of command list
        /// </summary>
        [[nodiscard]] uint32_t GetDrawIndexedCommandSize() const;

    public:
        /// <summary>
        /// Get the clear color.
        /// </summary>
        [[nodiscard]] const Math::Color4& GetClearColor() const noexcept;

        /// <summary>
        /// Set the clear color.
        /// </summary>
        void SetClearColor(
            const Math::Color4& color);

        /// <summary>
        /// Get the clear type for the backbuffer.
        /// </summary>
        [[nodiscard]] BackbufferClearType GetBackbufferClearType() const noexcept;

        /// <summary>
        /// Set the clear type for the backbuffer.
        /// </summary>
        void SetBackbufferClearType(
            BackbufferClearType type);

    public:
        /// <summary>
        /// The window of the device
        /// </summary>
        [[nodiscard]] Windowing::Window& GetWindow() const;

        /// <summary>
        /// Get the number of backbuffers.
        /// This is the number of backbuffers that are used for rendering.
        /// </summary>
        [[nodiscard]] uint8_t GetBackbufferCount() const;

        /// <summary>
        /// Get the index of the current backbuffer.
        /// This is the index of the backbuffer that is currently being rendered to.
        /// </summary>
        [[nodiscard]] uint8_t GetBackbufferIndex() const;

        /// <summary>
        /// Get the backbuffer at the specified index.
        /// </summary>
        [[nodiscard]] const Backbuffer& GetBackbuffer(
            uint8_t index) const;

        /// <summary>
        /// Get the current backbuffer.
        /// </summary>
        [[nodiscard]] const Backbuffer& GetBackbuffer() const;

        /// <summary>
        /// Get the backbuffer description.
        /// </summary>
        [[nodiscard]] const TextureDesc& GetBackBufferDesc() const;

    public:
        /// <summary>
        /// Return vsync state for our swapchain
        /// </summary>
        [[nodiscard]] bool IsVSyncEnabled() const noexcept;

        /// <summary>
        /// Return vsync state for our swapchain
        /// </summary>
        void SetVSyncEnabled(
            bool state = true);

    public:
        /// <summary>
        /// Process all events.
        /// This should be called once per frame.
        /// Returns false if the window is closed.
        /// </summary>
        [[nodiscard]] bool ProcessEvents();

        /// <summary>
        /// Begin a new frame.
        /// This should be called once per frame.
        /// This will reset the frame manager.
        /// </summary>
        void BeginFrame();

        /// <summary>
        /// End the current frame.
        /// This should be called once per frame.
        /// This will present the frame.
        /// </summary>
        void EndFrame();

    public:
        /// <summary>
        /// Idle the GPU.
        /// </summary>
        void WaitIdle();

        /// <summary>
        /// Clean up device resources cache.
        /// </summary>
        void CleanupCache();

    public:
        /// <summary>
        /// Create pipeline layout.
        /// </summary>
        [[nodiscard]] Co::result<Ptr<PipelineLayout>> CreatePipelineLayout(
            Co::executor_tag,
            Co::executor&             executor,
            const PipelineLayoutDesc& desc);

        /// <summary>
        /// Create pipeline layout.
        /// </summary>
        [[nodiscard]] Ptr<PipelineLayout> CreatePipelineLayout(
            const PipelineLayoutDesc& desc);

    private:
        /// <summary>
        /// Set the pipeline layout name.
        /// </summary>
        void SetName(
            nri::PipelineLayout& layout,
            const char*          name) const;

        /// <summary>
        /// Get the nri pipeline layout.
        /// </summary>
        void Release(
            nri::PipelineLayout& layout);

    public:
        /// <summary>
        /// Create graphics pipeline state.
        /// </summary>
        [[nodiscard]] Co::result<Ptr<PipelineState>> CreatePipelineState(
            Co::executor_tag,
            Co::executor&,
            const GraphicsPipelineDesc& desc);

        /// <summary>
        /// Create compute pipeline state.
        /// </summary>
        [[nodiscard]] Co::result<Ptr<PipelineState>> CreatePipelineState(
            Co::executor_tag,
            Co::executor&,
            const ComputePipelineDesc& desc);

        /// <summary>
        /// Create graphics pipeline state.
        /// </summary>
        [[nodiscard]] Ptr<PipelineState> CreatePipelineState(
            const GraphicsPipelineDesc& desc);

        /// <summary>
        /// Create compute pipeline state.
        /// </summary>
        [[nodiscard]] Ptr<PipelineState> CreatePipelineState(
            const ComputePipelineDesc& desc);

    private:
        /// <summary>
        /// Set the pipeline state name.
        /// </summary>
        void SetName(
            nri::Pipeline& nriPipeline,
            const char*    name) const;

        /// <summary>
        /// Get the nri pipeline state.
        /// </summary>
        void Release(
            nri::Pipeline& nriPipeline);

        // Below are the functions that are only accessible by the Texture
    private:
        /// <summary>
        /// Create a texture.
        /// </summary>
        nri::Texture* Create(
            MemoryLocation     location,
            const TextureDesc& desc);

        /// <summary>
        /// Releases the texture.
        /// </summary>
        void Release(
            nri::Texture& nriTexture);

        // Below are the functions that are only accessible by the Buffer
    private:
        /// <summary>
        /// Create a buffer.
        /// </summary>
        nri::Buffer* Create(
            MemoryLocation    location,
            const BufferDesc& desc);

        /// <summary>
        /// Releases the buffer.
        /// </summary>
        void Release(
            nri::Buffer& nriBuffer);

        // Below are the functions that are only accessible by the ResourceView
    private:
        /// <summary>
        /// Releases the resource view.
        /// </summary>
        void Release(
            nri::Descriptor& nriDescriptor);

    private:
        /// <summary>
        /// Get the implementation class.
        /// </summary>
        [[nodiscard]] DeviceImpl& GetImpl() const;

    private:
        UPtr<DeviceImpl> m_Impl;
    };
} // namespace Ame::Rhi