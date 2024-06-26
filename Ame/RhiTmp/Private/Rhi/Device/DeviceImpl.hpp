#pragma once

#include <Core/Ame.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/FrameManager.hpp>
#include <Rhi/Device/MemoryAllocator.hpp>
#include <Rhi/Device/StateTracker.hpp>

#include <Rhi/Hash/Layout.hpp>
#include <Rhi/Hash/Pipeline.hpp>
#include <Rhi/Util/TypedCache.hpp>

#include <Rhi/Nri/Bridge.hpp>

namespace Ame::Windowing
{
    class Window;
} // namespace Ame::Windowing

namespace Ame::Rhi
{
    class WindowManager;

    class DeviceImpl
    {
    private:
        using PipelineLayouCacheMap         = Util::TypedCache<PipelineLayoutDesc, Ptr<PipelineLayout>>;
        using GraphicsPipelineStateCacheMap = Util::TypedCache<GraphicsPipelineDesc, Ptr<PipelineState>>;
        using ComputePipelineStateCacheMap  = Util::TypedCache<ComputePipelineDesc, Ptr<PipelineState>>;

        friend class Device;

    public:
        explicit DeviceImpl(
            const DeviceCreateDesc& desc);

        ~DeviceImpl();

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

    public:
        /// <summary>
        /// Return vsync state for our swapchain
        /// </summary>
        [[nodiscard]] bool IsVSyncEnabled() const noexcept;

        /// <summary>
        /// Return vsync state for our swapchain
        /// </summary>
        void SetVSyncEnabled(
            bool State = true);

        /// <summary>
        /// Clean up device resources cache.
        /// </summary>
        void CleanupCache();

    public:
        /// <summary>
        /// Process all events.
        /// This should be called once per frame.
        /// Returns false if the window is closed.
        /// </summary>
        [[nodiscard]] bool ProcessEvents() const;

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

    public:
        /// <summary>
        /// Get the nri bridge.
        /// </summary>
        [[nodiscard]] NRIBridge& GetNRI() noexcept;

        /// <summary>
        /// Get the nri device.
        /// </summary>
        [[nodiscard]] nri::Device& GetDevice() noexcept;

        /// <summary>
        /// Get the nri command queue.
        /// </summary>
        [[nodiscard]] nri::CommandQueue& GetQueue() noexcept;

        /// <summary>
        /// Get the current command list.
        /// </summary>
        [[nodiscard]] class CommandListImpl& GetCurrentCommandList() noexcept;

        /// <summary>
        /// Get the resource state tracker for transitions
        /// </summary>
        [[nodiscard]] ResourceStateTracker& GetStateTracker() noexcept;

    public:
        /// <summary>
        /// Begin tracking a buffer
        /// </summary>
        void BeginTracking(
            nri::Buffer*     nriBuffer,
            nri::AccessStage initialState);

        /// <summary>
        /// Begin tracking a texture
        /// </summary>
        void BeginTracking(
            nri::Texture*          nriTexture,
            nri::AccessLayoutStage initialState);

        /// <summary>
        /// End tracking a buffer
        /// </summary>
        void EndTracking(
            nri::Buffer* nriBuffer);

        /// <summary>
        /// End tracking a texture
        /// </summary>
        void EndTracking(
            nri::Texture* nriTexture);

    public:
        /// <summary>
        /// Create resource view for a texture.
        /// </summary>
        [[nodiscard]] nri::Descriptor* CreateView(
            nri::Texture&          nriTexture,
            const TextureViewDesc& desc) const;

        /// <summary>
        /// Create resource view for a buffer.
        /// </summary>
        [[nodiscard]] nri::Descriptor* CreateView(
            nri::Buffer&          nriBuffer,
            const BufferViewDesc& desc) const;

    public:
        /// <summary>
        /// Release of buffer.
        /// </summary>
        void Release(
            nri::Buffer& nriBuffer);

        /// <summary>
        /// Defer the release of a texture.
        /// </summary>
        void Release(
            nri::Texture& nriTexture);

        /// <summary>
        /// Defer the release of a descriptor.
        /// </summary>
        void Release(
            nri::Descriptor& nriDescriptor);

        /// <summary>
        /// Defer the release of a pipeline state.
        /// </summary>
        void DeferRelease(
            nri::Pipeline& nriPipeline);

    private:
        /// <summary>
        /// Register the backbuffer state.
        /// </summary>
        void RegisterBackbufferState();

        /// <summary>
        /// Unregister the backbuffer state.
        /// </summary>
        void UnregisterBackbufferState();

        /// <summary>
        /// Transition the backbuffer to the specified state (either presenting or rendering)
        /// </summary>
        void TransitionBackbuffer(
            bool presenting);

        /// <summary>
        /// Clear the backbuffer with the specified color
        /// </summary>
        void ClearBackbuffer(
            const Math::Color4& color);

    private:
        /// <summary>
        /// Attempts to create the device.
        /// </summary>
        [[nodiscard]] bool CreateDevice(
            const DeviceCreateDesc& desc);

        /// <summary>
        /// Clears all resources used by the device.
        /// </summary>
        void ClearResources();

        /// <summary>
        /// Suppresses warnings if needed.
        /// This is used for debugging purposes.
        /// </summary>
        void SuppressWarningsIfNeeded(
            const DeviceCreateDesc& desc);

        /// <summary>
        /// Enables validation if needed.
        /// </summary>
        void EnableValidationIfNeeded(
            const DeviceCreateDesc& desc);

    private:
        NRIBridge            m_NRI;
        UPtr<WindowManager>  m_WindowManager;
        FrameManager         m_FrameManager;
        MemoryAllocator      m_MemoryAllocator;
        ResourceStateTracker m_ResourceStateTracker;

        nri::Device*       m_Device       = nullptr;
        nri::CommandQueue* m_CommandQueue = nullptr;

        Math::Color4        m_ClearColor = Colors::c_Magenta;
        BackbufferClearType m_ClearType  = BackbufferClearType::Color;

        PipelineLayouCacheMap         m_PipelineLayoutCache;
        GraphicsPipelineStateCacheMap m_GraphicsPipelineCache;
        ComputePipelineStateCacheMap  m_ComputePipelineCache;

        uint32_t m_DrawIndexedCommandSize = 0;
    };
} // namespace Ame::Rhi