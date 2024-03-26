#include <Engine/Engine.hpp>

#include <Log/Wrapper.hpp>

namespace Ame
{
    void AmeEngine::Run()
    {
        Initialize();

        while (m_IsRunning)
        {
        }

        Shutdown();
    }

    void AmeEngine::Close()
    {
        m_IsRunning = false;
    }

    //

    void AmeEngine::Initialize()
    {
    }

    void AmeEngine::Shutdown()
    {
    }
} // namespace Ame
