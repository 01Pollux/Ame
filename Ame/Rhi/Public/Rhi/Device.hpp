#pragma once

#include <Rhi/Core.hpp>
#include <Rhi/Backbuffer.hpp>

namespace Ame::Windowing
{
    class Window;
} // namespace Ame::Windowing

namespace Ame::Rhi
{
    struct DeviceCreateDesc;

    class Device : public NonCopyable
    {
        class Impl;

        friend Buffer;
        friend Texture;
        friend ResourceView;

    public:
        Device(
            const DeviceCreateDesc& Desc);

        Device(Device&&)            = default;
        Device& operator=(Device&&) = default;

        ~Device();

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

        // Below are the functions that are only accessible by the Buffer, Texture and ResourceView classes.
        // Texture
    private:
        /// <summary>
        /// Create a texture.
        /// </summary>
        nri::Texture* Create(
            const TextureDesc& Desc);

        /// <summary>
        /// Releases the texture.
        /// </summary>
        void Release(
            nri::Texture& Tex,
            bool          Defer);

        /// <summary>
        /// Set the texture name.
        /// </summary>
        void SetName(
            nri::Texture& Tex,
            const char*   Name);

        /// <summary>
        /// Get the texture desc.
        /// </summary>
        [[nodiscard]] const TextureDesc& GetDesc(
            nri::Texture& Tex) const;

        /// <summary>
        /// Get the underlying texture.
        /// </summary>
        [[nodiscard]] void* GetNative(
            nri::Texture& Tex) const;

        /// <summary>
        /// Create resource view for a texture.
        /// </summary>
        [[nodiscard]] nri::Descriptor* CreateView(
            nri::Texture&          Tex,
            const TextureViewDesc& Desc) const;

        // Buffer
    private:
        /// <summary>
        /// Create a buffer.
        /// </summary>
        nri::Buffer* Create(
            const BufferDesc& Desc);

        /// <summary>
        /// Releases the buffer.
        /// </summary>
        void Release(
            nri::Buffer& Buf,
            bool         Defer);

        /// <summary>
        /// Set the buffer name.
        /// </summary>
        void SetName(
            nri::Buffer& Buf,
            const char*  Name);

        /// <summary>
        /// Get the buffer desc.
        /// </summary>
        [[nodiscard]] const BufferDesc& GetDesc(
            nri::Buffer& Buf) const;

        /// <summary>
        /// Get the underlying buffer.
        /// </summary>
        [[nodiscard]] void* GetNative(
            nri::Buffer& Buf) const;

        /// <summary>
        /// Create resource view for a buffer.
        /// </summary>
        [[nodiscard]] nri::Descriptor* CreateView(
            nri::Buffer&          Buf,
            const BufferViewDesc& Desc) const;

        // ResourceView
    private:
        /// <summary>
        /// Releases the resource view.
        /// </summary>
        void Release(
            nri::Descriptor& View,
            bool             Defer);

        /// <summary>
        /// Set the resource view name.
        /// </summary>
        void SetName(
            nri::Descriptor& View,
            const char*      Name);

        /// <summary>
        /// Get the underlying resource view.
        /// </summary>
        [[nodiscard]] void* GetNative(
            nri::Descriptor& View) const;

    public:
        /// <summary>
        /// Idle the GPU.
        /// </summary>
        void WaitIdle();

    private:
        UPtr<Impl> m_Impl;
    };
} // namespace Ame::Rhi