#pragma once

#include <Core/Coroutine.hpp>
#include <Rhi/Wrapper/DeviceWrapper.hpp>

namespace Ame::Rhi
{
    struct DeviceCreateDesc;

    enum class ExecutorType : uint8_t
    {
        Primary, // Executor for the main rendering operations

        Graphics,  // Executor for graphics commands
        Compute,   // Executor for compute commands
        Copy,      // Executor for copy commands
        Resources, // Executor for resource creation and destruction

        Count
    };

    class RhiDevice
    {
    private:
        using DeviceThreadExecutors = std::array<Ptr<Co::manual_executor>, static_cast<size_t>(ExecutorType::Count)>;

    public:
        RhiDevice();
        explicit RhiDevice(
            Co::runtime&            runtime,
            const DeviceCreateDesc& createDesc);

        RhiDevice(const RhiDevice&) = delete;
        RhiDevice(RhiDevice&&)      = delete;

        RhiDevice& operator=(const RhiDevice&) = delete;
        RhiDevice& operator=(RhiDevice&&)      = delete;

        ~RhiDevice();

    public:
        /// <summary>
        /// Check if the device is null (without graphic dev ice and window)
        /// </summary>
        [[nodiscard]] bool IsNull() const noexcept;

        /// <summary>
        /// Check if the device is headless (without window)
        /// </summary>
        [[nodiscard]] bool IsHeadless() const noexcept;

        /// <summary>
        /// Check if the device has window.
        /// </summary>
        [[nodiscard]] bool HasSurface() const noexcept;

        /// <summary>
        /// Get the device coroutine executor.
        /// </summary>
        [[nodiscard]] const Ptr<Co::manual_executor>& GetExecutor(
            ExecutorType type = ExecutorType::Primary) const;

    public:
        /// <summary>
        /// Get the graphics API used by the device.
        /// </summary>
        [[nodiscard]] Dg::RENDER_DEVICE_TYPE GetGraphicsAPI() const;

        /// <summary>
        /// Get the graphics API used by the device.
        /// </summary>
        [[nodiscard]] const char* GetGraphicsAPIName() const;

    public:
        /// <summary>
        /// Get the engine factory.
        /// </summary>
        [[nodiscard]] Dg::IEngineFactory* GetFactory() const;

        /// <summary>
        /// Get the graphics device.
        /// </summary>
        [[nodiscard]] Dg::IRenderDevice* GetDevice() const;

        /// <summary>
        /// Get the device immediate context.
        /// </summary>
        [[nodiscard]] Dg::IDeviceContext* GetImmediateContext() const;

        /// <summary>
        /// Get the swapchain window.
        /// </summary>
        [[nodiscard]] Dg::ISwapChain* GetSwapchain() const;

        /// <summary>
        /// Get the window.
        /// </summary>
        [[nodiscard]] Ptr<Windowing::Window> GetWindow() const;

    public:
        /// <summary>
        /// Run the tasks that have been submitted to the device.
        /// </summary>
        [[nodiscard]] bool ProcessTasks();

        /// <summary>
        /// End the rendering device frame and present if device has window.
        /// </summary>
        void AdvanceFrame(
            uint32_t syncInterval = 1);

    private:
        DeviceWrapper m_Wrapper;

        DeviceThreadExecutors m_Executors;

        uint32_t m_ConcurrentLoopCount;
        uint32_t m_ConcurrentLoopChunkSize;
    };
} // namespace Ame::Rhi