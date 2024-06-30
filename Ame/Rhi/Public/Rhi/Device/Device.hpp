#pragma once

#include <Core/Coroutine.hpp>
#include <Rhi/Descs/Core.hpp>

#include <Rhi/CommandList/SubmissionContext.hpp>

namespace Ame
{
    class EngineFrame;
    namespace Math
    {
        class Color4;
    } // namespace Math
    namespace Windowing
    {
        class Window;
    } // namespace Windowing
} // namespace Ame

namespace Ame::Rhi
{
    struct DeviceCreateDesc;

    enum class ExecutorType : uint8_t
    {
        Primary,   // Executor for the main thread
        Graphics,  // Executor for graphics commands
        Compute,   // Executor for compute commands
        Copy,      // Executor for copy commands
        Resources, // Executor for resource creation and destruction

        Count
    };

    class Device
    {
    public:
        Device();
        explicit Device(
            Co::runtime&            runtime,
            const DeviceCreateDesc& desc);

        Device(const Device&) = delete;
        Device(Device&&)      = delete;

        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&)      = delete;

        ~Device();

    public:
        /// <summary>
        /// Check if the device is headless.
        /// </summary>
        [[nodiscard]] bool IsHeadless() const noexcept;

        /// <summary>
        /// Get the device coroutine executor.
        /// </summary>
        [[nodiscard]] Ptr<Co::executor> GetExecutor(
            ExecutorType type = ExecutorType::Primary) const;

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
        [[nodiscard]] Co::result<void> WaitIdle();

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
        [[nodiscard]] DeviceWindowManager& GetWindowManager();

    private:
        UPtr<DeviceImpl> m_Impl;
    };
} // namespace Ame::Rhi