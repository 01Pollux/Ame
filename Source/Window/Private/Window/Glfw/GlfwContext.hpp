#pragma once

#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>

#include <Window/Glfw/GlfwHooks.hpp>

namespace Ame::Window
{
    /// <summary>
    /// Since glfw is a singleton, we need to initialize it before using it
    /// This class is dedicated to initializing and handling all glfw operations in a thread-safe manner
    /// </summary>
    class GlfwContext
    {
    public:
        using Task = std::move_only_function<void()>;

    public:
        /// <summary>
        /// Initialize the glfw context, pass a previous context to transfer the ownership of the glfw context
        /// </summary>
        static void Initialize();

        /// <summary>
        /// Get the glfw context
        /// </summary>
        [[nodiscard]] static GlfwContext& Get();

        [[nodiscard]] static GlfwHooks& GetHooks();

    public:
        GlfwContext();

        GlfwContext(const GlfwContext&) = delete;
        GlfwContext(GlfwContext&&)      = delete;

        GlfwContext& operator=(GlfwContext&&)      = delete;
        GlfwContext& operator=(const GlfwContext&) = delete;

        ~GlfwContext();

    public:
        /// <summary>
        /// Push a task to the glfw worker thread
        /// </summary>
        template<std::invocable Fn> [[nodiscard]] auto PushTask(Fn callback)
        {
            using ResTy = std::invoke_result_t<Fn>;
            std::packaged_task<ResTy()> pkg{ std::forward<Fn>(callback) };

            auto future = pkg.get_future();
            PushWrappedTask([pkg = std::move(pkg)]() mutable { pkg(); });
            return future;
        }

    private:
        /// <summary>
        /// Push a task to the glfw worker thread
        /// </summary>
        void PushWrappedTask(Task task);

    private:
        /// <summary>
        /// Initialize the glfw worker thread
        /// </summary>
        void InitializeGlfwWorker();

        /// <summary>
        /// The glfw worker thread
        /// </summary>
        void GlfwWorker();

        /// <summary>
        /// Shutdown the glfw worker thread
        /// </summary>
        void ShutdownGlfwWorker();

    private:
        std::jthread m_GlfwThread;

        std::mutex              m_TaskMutex;
        std::queue<Task>        m_Tasks;
        std::condition_variable m_TaskNotifier;

        GlfwHooks m_Hooks;
    };
} // namespace Ame::Window