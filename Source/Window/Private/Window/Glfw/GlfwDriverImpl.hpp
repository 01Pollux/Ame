#pragma once

#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>

#include <Window/Glfw/GlfwHooks.hpp>
#include <Window/GlfwDriver.hpp>

namespace Ame::Window
{
    /// <summary>
    /// Since glfw is a singleton, we need to initialize it before using it
    /// This class is dedicated to initializing and handling all glfw operations in a thread-safe manner
    /// </summary>
    class GlfwDriverImpl : public IGlfwDriver
    {
    public:
        /// <summary>
        /// Initialize the glfw context, pass a previous context to transfer the ownership of the glfw context
        /// </summary>
        static SharedPtr<IGlfwDriver> Initialize();

        [[nodiscard]] static GlfwHooks& GetHooks();

    public:
        GlfwDriverImpl();

    public:
        void enqueue(Co::task task) override;
        void enqueue(std::span<Co::task> tasks) override;
        int  max_concurrency_level() const noexcept override
        {
            return 1;
        }
        bool shutdown_requested() const noexcept override;
        void shutdown() noexcept override;

    private:
        /// <summary>
        /// Initialize the glfw worker thread
        /// </summary>
        void InitializeGlfwWorker();

        /// <summary>
        /// The glfw worker thread
        /// </summary>
        void GlfwDispatcher();

        /// <summary>
        /// The glfw worker thread
        /// </summary>
        void GlfwProcessor();

        /// <summary>
        /// Shutdown the glfw worker thread
        /// </summary>
        void ShutdownGlfwWorker();

    private:
        std::jthread m_GlfwDispatcher;

        std::mutex              m_TaskMutex;
        std::queue<Co::task>    m_Tasks;
        std::condition_variable m_TaskNotifier;

        GlfwHooks m_Hooks;
    };
} // namespace Ame::Window