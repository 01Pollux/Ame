#include <Core/Coroutine.hpp>
#include <Window/Glfw/GlfwDriverImpl.hpp>
#include <mimalloc.h>

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

    IGlfwDriver* GlfwDriverImpl::Initialize()
    {
        if (!s_DriverInstance) [[unlikely]]
        {
            std::scoped_lock lock(s_DriverInitMutex);
            if (!s_DriverInstance)
            {
                s_DriverInstance = Coroutine::Get().make_executor<GlfwDriverImpl>();
            }
        }
        return s_DriverInstance.get();
    }

    GlfwHooks& GlfwDriverImpl::GetHooks()
    {
        return static_cast<GlfwDriverImpl*>(Initialize())->m_State->Hooks;
    }

    GlfwHooks& GlfwHooks::Get()
    {
        return GlfwDriverImpl::GetHooks();
    }

    GlfwDriverImpl::GlfwDriverImpl() : IGlfwDriver("GlfwDriver")
    {
        m_State->GlfwDispatcher = std::thread([this] { GlfwDispatcher(); });
    }

    //

    void GlfwDriverImpl::enqueue(Co::task task)
    {
        if (!m_State || m_State->StopRequested) [[unlikely]]
        {
            throw concurrencpp::errors::runtime_shutdown("Cannot enqueue task, the worker thread is shutting down");
        }

        {
            std::unique_lock lock(m_State->TaskMutex);

            if (m_State->Hooks.IsCallbackInProgress())
            {
                task();
            }
            else
            {
                m_State->Tasks.push(std::move(task));
            }
        }
        m_State->TaskNotifier.notify_one();
    }

    void GlfwDriverImpl::enqueue(std::span<Co::task> tasks)
    {
        if (!m_State || m_State->StopRequested) [[unlikely]]
        {
            throw concurrencpp::errors::runtime_shutdown("Cannot enqueue task, the worker thread is shutting down");
        }

        {
            std::unique_lock lock(m_State->TaskMutex);
            if (m_State->Hooks.IsCallbackInProgress())
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
                    m_State->Tasks.push(std::move(task));
                }
            }
        }
        m_State->TaskNotifier.notify_one();
    }

    bool GlfwDriverImpl::shutdown_requested() const noexcept
    {
        return !m_State;
    }

    void GlfwDriverImpl::shutdown() noexcept
    {
        {
            std::unique_lock lock(m_State->TaskMutex);
            m_State->StopRequested = true;
        }
        m_State->TaskNotifier.notify_one();
        m_State->GlfwDispatcher.join();
        m_State = nullptr;
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

        while (!m_State->StopRequested)
        {
            glfwPollEvents();

            Co::task task;
            {
                std::unique_lock lock(m_State->TaskMutex);
                m_State->TaskNotifier.wait_for(lock, std::chrono::milliseconds(1),
                                               [&]
                                               {
                                                   if (m_State->StopRequested)
                                                   {
                                                       return true;
                                                   }
                                                   if (!m_State->Tasks.empty())
                                                   {
                                                       task = std::move(m_State->Tasks.front());
                                                       m_State->Tasks.pop();
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
                    m_State->Hooks.SetCallbackInProgress(true);
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
                m_State->Hooks.SetCallbackInProgress(true);
                task();
            }
#endif
            m_State->Hooks.SetCallbackInProgress(false);
        }

        ShutdownGlfwWorker();
    }

    void GlfwDriverImpl::ShutdownGlfwWorker()
    {
        glfwTerminate();
    }
} // namespace Ame::Window