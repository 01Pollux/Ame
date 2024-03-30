#pragma once

#include <Rhi/Device.hpp>
#include "FrameManager.hpp"
#include "../Nri/Bridge.hpp"

namespace Ame::Windowing
{
    class Window;
} // namespace Ame::Windowing

namespace Ame::Rhi
{
    class WindowManager;

    class Device::Impl : public NonCopyable, public NonMovable
    {
    public:
        Impl(
            const DeviceCreateDesc& Desc);

        ~Impl();

    public:
        /// <summary>
        /// Get the graphics API used by the device.
        /// </summary>
        [[nodiscard]] GraphicsAPI GetGraphicsAPI() const;

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

    public:
        /// <summary>
        /// Get the clear color.
        /// </summary>
        [[nodiscard]] const Math::Color4& GetClearColor() const noexcept;

        /// <summary>
        /// Set the clear color.
        /// </summary>
        void SetClearColor(
            const Math::Color4& Color);

        /// <summary>
        /// Get the clear type for the backbuffer.
        /// </summary>
        [[nodiscard]] BackbufferClearType GetBackbufferClearType() const noexcept;

        /// <summary>
        /// Set the clear type for the backbuffer.
        /// </summary>
        void SetBackbufferClearType(
            BackbufferClearType Type);

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
        [[nodiscard]] Backbuffer GetBackbuffer(
            uint8_t Index) const;

        /// <summary>
        /// Get the current backbuffer.
        /// </summary>
        [[nodiscard]] Backbuffer GetBackbuffer() const;

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
        /// Get the current command list.
        /// </summary>
        [[nodiscard]] nri::CommandBuffer& GetCurrentCommandList() const noexcept;

    public:
        /// <summary>
        /// Begin tracking a buffer
        /// </summary>
        void BeginTracking(
            nri::Buffer*     Buffer,
            nri::AccessStage InitialState);

        /// <summary>
        /// Begin tracking a texture
        /// </summary>
        void BeginTracking(
            nri::Texture*          Texture,
            nri::AccessLayoutStage InitialState);

        /// <summary>
        /// End tracking a buffer
        /// </summary>
        void EndTracking(
            nri::Buffer* Buffer);

        /// <summary>
        /// End tracking a texture
        /// </summary>
        void EndTracking(
            nri::Texture* Texture);

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
            bool Presenting);

        /// <summary>
        /// Clear the backbuffer with the specified color
        /// </summary>
        void ClearBackbuffer(
            const Math::Color4& Color);

    private:
        /// <summary>
        /// Attempts to create the device.
        /// </summary>
        [[nodiscard]] bool CreateDevice(
            const DeviceCreateDesc& Desc);

        /// <summary>
        /// Clears all resources used by the device.
        /// </summary>
        void ClearResources();

        /// <summary>
        /// Suppresses warnings if needed.
        /// This is used for debugging purposes.
        /// </summary>
        void SuppressWarningsIfNeeded(
            const DeviceCreateDesc& Desc);

    private:
        NRIBridge            m_NRI;
        UPtr<WindowManager>  m_WindowManager;
        FrameManager         m_FrameManager;
        ResourceStateTracker m_ResourceStateTracker;

        nri::Device*       m_Device       = nullptr;
        nri::CommandQueue* m_CommandQueue = nullptr;

        Math::Color4        m_ClearColor = Math::Colors::Magenta;
        BackbufferClearType m_ClearType  = BackbufferClearType::Color;
    };
} // namespace Ame::Rhi