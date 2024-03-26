#include <Engine/Engine.hpp>

#include <Log/Wrapper.hpp>

namespace Ame
{
    void BaseEngine::Run()
    {
        Initialize();

        while (m_IsRunning)
        {
        }

        Shutdown();
    }

    void BaseEngine::Close()
    {
        m_IsRunning = false;
    }

    //

    void BaseEngine::Initialize()
    {
    }

    void BaseEngine::Shutdown()
    {
    }
} // namespace Ame
