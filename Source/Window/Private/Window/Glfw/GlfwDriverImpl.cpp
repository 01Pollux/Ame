#include <Window/Glfw/GlfwDriverImpl.hpp>
#include <GLFW/glfw3.h>

#include <Log/Logger.hpp>

namespace Ame::Window
{
    static SharedPtr<IGlfwDriver> s_DriverInstance;
    static std::mutex             s_DriverInitMutex;

    static void GlfwErrorCallback(int code, const char* description)
    {
        AME_LOG_ERROR(std::format("GLFW Error: {} ({})", description, code));
    }

    //

    SharedPtr<IGlfwDriver> GlfwDriverImpl::Initialize()
    {
        if (!s_DriverInstance) [[unlikely]]
        {
            std::scoped_lock lock(s_DriverInitMutex);
            if (!s_DriverInstance)
            {
                s_DriverInstance = std::make_shared<GlfwDriverImpl>();
            }
        }
        return s_DriverInstance;
    }

    GlfwHooks& GlfwDriverImpl::GetHooks()
    {
        return static_cast<GlfwDriverImpl*>(Initialize().get())->m_Hooks;
    }

    GlfwHooks& GlfwHooks::Get()
    {
        return GlfwDriverImpl::GetHooks();
    }

    GlfwDriverImpl::GlfwDriverImpl() : IGlfwDriver("GlfwDriver")
    {
        m_GlfwDispatcher = std::jthread(&GlfwDriverImpl::GlfwDispatcher, this);
    }

    //

    void GlfwDriverImpl::enqueue(Co::task task)
    {
        if (m_GlfwDispatcher.get_stop_token().stop_requested()) [[unlikely]]
        {
            throw concurrencpp::errors::runtime_shutdown("Cannot enqueue task, the worker thread is shutting down");
        }

        {
            std::unique_lock lock(m_TaskMutex);

            if (m_Hooks.IsCallbackInProgress())
            {
                task();
            }
            else
            {
                m_Tasks.push(std::move(task));
            }
        }
        m_TaskNotifier.notify_one();
    }

    void GlfwDriverImpl::enqueue(std::span<Co::task> tasks)
    {
        if (m_GlfwDispatcher.get_stop_token().stop_requested()) [[unlikely]]
        {
            throw concurrencpp::errors::runtime_shutdown("Cannot enqueue task, the worker thread is shutting down");
        }

        {
            std::unique_lock lock(m_TaskMutex);
            if (m_Hooks.IsCallbackInProgress())
            {
                for (auto& task : tasks)
                {
                    task();
                }
            }
            else
            {
                for (auto& task : tasks)
                {
                    m_Tasks.push(std::move(task));
                }
            }
        }
        m_TaskNotifier.notify_one();
    }

    bool GlfwDriverImpl::shutdown_requested() const noexcept
    {
        return m_GlfwDispatcher.get_stop_token().stop_requested();
    }

    void GlfwDriverImpl::shutdown() noexcept
    {
        m_GlfwDispatcher.request_stop();
        {
            std::unique_lock lock(m_TaskMutex);
            m_Tasks.push([] { glfwPostEmptyEvent(); });
        }
        m_TaskNotifier.notify_one();
        m_GlfwDispatcher.join();
    }

    //

    void GlfwDriverImpl::InitializeGlfwWorker()
    {
        GLFWallocator glfwAllocator{ .allocate = [](size_t size, void*) { return mi_malloc(size); },
                                     .reallocate = [](void* block, size_t size, void*)
                                     { return mi_realloc(block, size); },
                                     .deallocate = [](void* block, void*) { mi_free(block); } };

        glfwSetErrorCallback(GlfwErrorCallback);
        glfwInitAllocator(&glfwAllocator);
        AME_LOG_ASSERT(glfwInit() == GLFW_TRUE, "Failed to initialize GLFW");
    }

    void GlfwDriverImpl::GlfwDispatcher()
    {
        InitializeGlfwWorker();

        auto stopToken = m_GlfwDispatcher.get_stop_token();
        while (!stopToken.stop_requested())
        {
            glfwPollEvents();

            Co::task task;
            {
                std::unique_lock lock(m_TaskMutex);
                m_TaskNotifier.wait_for(lock, std::chrono::milliseconds(1),
                                        [&]
                                        {
                                            if (stopToken.stop_requested())
                                            {
                                                return true;
                                            }
                                            if (!m_Tasks.empty())
                                            {
                                                task = std::move(m_Tasks.front());
                                                m_Tasks.pop();
                                                return true;
                                            }
                                            return false;
                                        });
            }

#if defined(AME_DEBUG) && !defined(AME_NO_EXCEPTIONS)
            try
            {
                if (task) [[likely]]
                {
                    m_Hooks.SetCallbackInProgress(true);
                    task();
                }
            }
            catch (const std::exception& e)
            {
                AME_LOG_ERROR(std::format("Unhandled exception in GLFW worker thread: {}", e.what()));
            }
#else
            if (task) [[likely]]
            {
                m_TaskInProgress = true;
                task();
            }
#endif
            m_Hooks.SetCallbackInProgress(false);
        }

        ShutdownGlfwWorker();
    }

    void GlfwDriverImpl::ShutdownGlfwWorker()
    {
        glfwTerminate();
    }
} // namespace Ame::Window