#include <Application/Application.hpp>
#include <Engine/Engine.hpp>

#include <Log/Wrapper.hpp>

namespace Ame
{
    BareApplication::BareApplication(
        int    argc,
        char** argv)
    {
        m_Engine = std::make_unique<AmeEngine>();
    }

    BareApplication::~BareApplication()
    {
        Log::Engine().Trace("Application destroyed");
    }

    //

    int BareApplication::Run()
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
            OnUnload();
#if !(defined(AME_DIST) || defined(AME_NO_EXCEPTIONS))
        }
        catch (const std::bad_alloc& e)
        {
            Log::Engine().Fatal("Memory allocation failed: {}", e.what());
            return EXIT_FAILURE;
        }
#endif
        return EXIT_SUCCESS;
    }
} // namespace Ame
