#include <Application/Application.hpp>
#include <Engine/Engine.hpp>

#include <Log/Logger.hpp>

namespace Ame
{
    BaseApplication::BaseApplication(const ApplicationConfig& applicationConfig) :
        m_Engine(std::make_unique<AmeEngine>(applicationConfig.Engine))
    {
    }

    BaseApplication::~BaseApplication()
    {
        AME_LOG_TRACE("Application destroyed");
    }

    //

    int BaseApplication::Run()
    {
#if !(defined(AME_DIST) || defined(AME_NO_EXCEPTIONS))
        //try
        {
#endif
            AME_LOG_TRACE("Application started");
            OnLoad();
            AME_LOG_TRACE("Application loaded");
            OnInitialize();
            AME_LOG_TRACE("Application initialized");

            while (m_Engine->IsRunning())
            {
                m_Engine->Tick();
            }

            OnShutdown();
            AME_LOG_TRACE("Application shutdown");
            OnUnload();
            AME_LOG_TRACE("Application unloaded");
            m_Engine = nullptr;
#if !(defined(AME_DIST) || defined(AME_NO_EXCEPTIONS))
        }
        //catch (const std::exception& e)
        //{
        //    AME_LOG_FATAL(std::format("Unhandled exception: {}", e.what()));
        //    m_Engine = nullptr;
        //    return EXIT_FAILURE;
        //}
#endif
        return EXIT_SUCCESS;
    }
} // namespace Ame
