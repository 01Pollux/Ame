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
        static IGlfwDriver* Initialize();

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
        struct State
        {
            std::thread GlfwDispatcher;

            std::mutex              TaskMutex;
            std::queue<Co::task>    Tasks;
            std::condition_variable TaskNotifier;
            bool                    StopRequested = false;

            GlfwHooks Hooks;
        };
        UniquePtr<State> m_State = std::make_unique<State>();
    };
} // namespace Ame::Window