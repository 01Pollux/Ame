#include <Application/Application.hpp>
#include <Engine/Engine.hpp>

#include <Log/Wrapper.hpp>

namespace Ame
{
    BaseApplication::BaseApplication(const ApplicationConfig& applicationConfig) :
        m_Engine(std::make_unique<AmeEngine>(applicationConfig.Engine))
    {
    }

    BaseApplication::~BaseApplication()
    {
        Log::Engine().Trace("Application destroyed");
    }

    //

    int BaseApplication::Run()
    {
        Log::Engine().Trace("Application started");
#if !(defined(AME_DIST) || defined(AME_NO_EXCEPTIONS))
        try
        {
#endif
            OnLoad();
            Log::Engine().Trace("Application loaded");
            OnInitialize();
            Log::Engine().Trace("Application initialized");

            while (m_Engine->IsRunning())
            {
                m_Engine->Tick();
            }

            OnShutdown();
            Log::Engine().Trace("Application shutdown");
            OnUnload();
            Log::Engine().Trace("Application unloaded");
#if !(defined(AME_DIST) || defined(AME_NO_EXCEPTIONS))
        }
        catch (const std::bad_alloc& e)
        {
            Log::Engine().Fatal("Memory allocation failed: {}", e.what());
            return EXIT_FAILURE;
        }
#endif
        Log::Engine().Trace("Application stopped");
        return EXIT_SUCCESS;
    }
} // namespace Ame
