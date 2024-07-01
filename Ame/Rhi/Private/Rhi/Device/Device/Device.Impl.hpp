#pragma once

#include <Core/Ame.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/Wrapper/DeviceWrapper.hpp>

#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/CommandSubmitter.hpp>
#include <Rhi/Device/WindowManager.hpp>

#include <Rhi/Device/Frame/FrameManager.hpp>
#include <Rhi/Device/CommandList/CommandListManager.hpp>

#include <Object/Signal.hpp>

namespace Ame
{
    class EngineFrame;
} // namespace Ame

namespace Ame::Rhi
{
    class WindowManager;

    class DeviceImpl
    {
        using DeviceThreadExecutors = std::array<Ptr<Co::manual_executor>, static_cast<size_t>(ExecutorType::Count)>;

    public:
        DeviceImpl(
            Co::runtime&            runtime,
            const DeviceCreateDesc& desc);

        DeviceImpl(const DeviceImpl&) = delete;
        DeviceImpl(DeviceImpl&&)      = delete;

        DeviceImpl& operator=(const DeviceImpl&) = delete;
        DeviceImpl& operator=(DeviceImpl&&)      = delete;

        ~DeviceImpl();

    public:
        /// <summary>
        /// Get the device coroutine executor.
        /// </summary>
        [[nodiscard]] const Ptr<Co::manual_executor>& GetExecutor(
            ExecutorType type = ExecutorType::Primary) const;

        /// <summary>
        /// Get the device wrapper.
        /// </summary>
        [[nodiscard]] IDeviceWrapper& GetDeviceWrapper() const noexcept;

        /// <summary>
        /// Get the frame manager.
        /// </summary>
        [[nodiscard]] FrameManager& GetFrameManager() noexcept;

        /// <summary>
        /// Get the window manager.
        /// </summary>
        [[nodiscard]] WindowManager* GetWindowManager() noexcept;

        /// <summary>
        /// Get the command list manager.
        /// </summary>
        [[nodiscard]] CommandListManager& GetCommandListManager() noexcept;

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
        /// Helper function to get the size of draw indexed command size for indirect execution of command list
        /// </summary>
        [[nodiscard]] uint32_t GetDrawIndexedCommandSize() const;

    public:
        /// <summary>
        /// Begin the rendering device frame.
        /// Returns true if the frame was started successfully.
        /// </summary>
        [[nodiscard]] bool BeginFrame();

        /// <summary>
        /// Run the tasks that have been submitted to the device.
        /// </summary>
        void ProcessTasks();

        /// <summary>
        /// End the rendering device frame.
        /// </summary>
        void EndFrame();

        /// <summary>
        /// Idle the GPU.
        /// </summary>
        void WaitIdle();

    public:
        /// <summary>
        /// Get the device resource allocator.
        /// </summary>
        [[nodiscard]] DeviceResourceAllocator& GetResourceAllocator();

        /// <summary>
        /// Get the device command submitter.
        /// </summary>
        [[nodiscard]] DeviceCommandSubmitter& GetCommandSubmitter();

        /// <summary>
        /// Get the device window manager.
        /// </summary>
        [[nodiscard]] DeviceWindowManager& GetDeviceWindowManager();

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

    private:
        /// <summary>
        /// Process all events.
        /// This should be called once per frame.
        /// Returns false if the window is closed.
        /// </summary>
        [[nodiscard]] bool ProcessEvents();

        /// <summary>
        /// Start the frame.
        /// </summary>
        void StartFrame();

        /// <summary>
        /// Update the frame.
        /// </summary>
        void FrameUpdate();

        /// <summary>
        /// End the frame.
        /// </summary>
        void FrameEnd();

    public:
        /// <summary>
        /// Assert that the current thread is the rendering thread.
        /// </summary>
        void AssertInRenderingThread(
            bool InThread = true) const;

    private:
        UPtr<IDeviceWrapper> m_DeviceWrapper;

        DeviceResourceAllocator m_ResourceAllocator;
        DeviceCommandSubmitter  m_CommandSubmitter;
        DeviceWindowManager     m_DeviceWindowManager;

        FrameManager        m_FrameManager;
        UPtr<WindowManager> m_WindowManager;
        CommandListManager  m_CommandListManager;
        SubmissionContext   m_PresentContext;

        DeviceThreadExecutors m_Executors;

        uint32_t m_ConcurrentLoopCount;
        uint32_t m_ConcurrentLoopChunkSize;

        uint32_t m_DrawIndexedCommandSize = 0;

        static inline std::thread::id s_RenderingThreadId{};
    };
} // namespace Ame::Rhi