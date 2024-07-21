#include <Window/Glfw/GlfwContext.hpp>
#include <GLFW/glfw3.h>

#include <Log/Wrapper.hpp>

namespace Ame::Window
{
    static std::shared_ptr<GlfwContext> s_Instance;
    static std::mutex                   s_GlfwInitMutex;

    static void GlfwErrorCallback(
        int         code,
        const char* description)
    {
        Log::Window().Error("GLFW Error: {0} ({1})", description, code);
    }

    //

    void GlfwContext::Initialize()
    {
        if (!s_Instance) [[unlikely]]
        {
            std::scoped_lock lock(s_GlfwInitMutex);
            if (!s_Instance)
            {
                s_Instance = std::make_shared<GlfwContext>();
            }
        }
    }

    GlfwContext& GlfwContext::Get()
    {
        return *s_Instance;
    }

    GlfwHooks& GlfwContext::GetHooks()
    {
        return Get().m_Hooks;
    }

    GlfwHooks& GlfwHooks::Get()
    {
        return GlfwContext::GetHooks();
    }

    GlfwContext::GlfwContext()
    {
        m_GlfwThread = std::jthread(&GlfwContext::GlfwWorker, this);
    }

    GlfwContext::~GlfwContext()
    {
        m_GlfwThread.request_stop();
        m_TaskNotifier.notify_one();
    }

    //

    void GlfwContext::PushWrappedTask(
        Task task)
    {
        {
            std::unique_lock lock(m_TaskMutex);
            // Push the task to the queue
            m_Tasks.push(std::move(task));
        }
        m_TaskNotifier.notify_one();
    }

    //

    void GlfwContext::InitializeGlfwWorker()
    {
        GLFWallocator glfwAllocator{
            .allocate = [](size_t size, void*)
            { return mi_malloc(size); },
            .reallocate = [](void* block, size_t size, void*)
            { return mi_realloc(block, size); },
            .deallocate = [](void* block, void*)
            { mi_free(block); }
        };

        glfwSetErrorCallback(GlfwErrorCallback);
        glfwInitAllocator(&glfwAllocator);
        AME_LOG_ASSERT(Log::Window(), glfwInit() == GLFW_TRUE, "Failed to initialize GLFW");
    }

    void GlfwContext::GlfwWorker()
    {
        InitializeGlfwWorker();

        auto stopToken = m_GlfwThread.get_stop_token();
        while (!stopToken.stop_requested())
        {
            Task task;
            {
                std::unique_lock lock(m_TaskMutex);
                m_TaskNotifier.wait(lock,
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
#ifdef AME_DEBUG
            try
            {
                if (task) [[likely]]
                {
                    task();
                }
            }
            catch (const std::exception& e)
            {
                Log::Window().Error("Unhandled exception in GLFW worker thread: {0}", e.what());
            }
#else
            if (task) [[likely]]
            {
                task();
            }
#endif
        }

        ShutdownGlfwWorker();
    }

    void GlfwContext::ShutdownGlfwWorker()
    {
        glfwTerminate();
    }
} // namespace Ame::Window