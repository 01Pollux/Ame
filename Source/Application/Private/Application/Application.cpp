#include <Application/Application.hpp>
#include <Engine/Engine.hpp>

#include <Log/Wrapper.hpp>

namespace Ame
{
    BareApplication::BareApplication()
    {
        m_Engine = std::make_unique<AmeEngine>();
    }

    int BareApplication::Run()
    {
        try
        {
            OnLoad();
            OnInitialize();

            while (m_Engine->IsRunning())
            {
                m_Engine->Tick();
            }

            Shutdown();
            OnUnload();
        }
        catch (const std::bad_alloc& e)
        {
            Log::Engine().Fatal("Memory allocation failed: {}", e.what());
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
} // namespace Ame
