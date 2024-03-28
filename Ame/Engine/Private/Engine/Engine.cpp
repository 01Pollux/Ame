#include <Engine/Engine.hpp>

#include <Log/Wrapper.hpp>

namespace Ame
{
    void BaseEngine::Run()
    {
        Log::Engine().Trace("Initializing Engine...");

        m_Logic.Initialize();
        Initialize();

        Log::Engine().Trace("Engine Initialized");

        while (m_Logic.IsRunning())
        {
            m_Logic.Tick();
        }

        Log::Engine().Trace("Shutting down Engine...");

        Shutdown();
        m_Logic.Shutdown();

        Log::Engine().Trace("Engine Shutdown");
    }

    void BaseEngine::Close()
    {
        m_Logic.Stop();
    }
} // namespace Ame
