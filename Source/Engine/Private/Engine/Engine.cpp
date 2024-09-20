#include <Engine/Engine.hpp>

#include <Interfaces/Core/FrameTimer.hpp>
#include <Interfaces/Core/FrameEvent.hpp>
#include <Interfaces/Ecs/EntityWorld.hpp>
#include <Interfaces/Graphics/Renderer.hpp>

#include <Log/Logger.hpp>

namespace Ame
{
    AmeEngine::AmeEngine() : m_ModuleRegistery(CreateModuleRegistry())
    {
    }

    AmeEngine::AmeEngine(const EngineConfig& engineConfig) : m_ModuleRegistery(CreateModuleRegistry())
    {
        engineConfig.ExposeInterfaces(m_ModuleRegistery.get(), nullptr);
        RefreshSubmoduleCache();
    }

    AmeEngine::~AmeEngine() = default;

    //

    void AmeEngine::RefreshSubmoduleCache()
    {
        m_ModuleRegistery->RequestInterface(nullptr, Interfaces::IID_FrameTimer, m_FrameTimer.DblPtr<IObject>());
        m_ModuleRegistery->RequestInterface(nullptr, Interfaces::IID_FrameEvent, m_FrameEvent.DblPtr<IObject>());
        m_ModuleRegistery->RequestInterface(nullptr, Interfaces::IID_EntityWorld, m_EntityWorld.DblPtr<IObject>());
        m_ModuleRegistery->RequestInterface(nullptr, Interfaces::IID_Renderer, m_Renderer.DblPtr<IObject>());
    }

    //

    void AmeEngine::Tick()
    {
        bool shouldQuit = false;

        AME_LOG_ASSERT(m_FrameTimer != nullptr, "FrameTimer was not registered");
        m_FrameTimer->Tick();

        AME_LOG_ASSERT(m_FrameEvent != nullptr, "FrameTimer was not registered");
        m_FrameEvent->OnFrameStart.Invoke();
        m_FrameEvent->OnFrameUpdate.Invoke();

        if (m_EntityWorld)
        {
            shouldQuit |= !m_EntityWorld->Progress(m_FrameTimer->GetDeltaTime());
        }

        if (m_Renderer)
        {
            shouldQuit |= !m_Renderer->Tick();
        }

        shouldQuit |= !m_FrameEvent->OnFrameEnd.Invoke().value_or(true);

        if (shouldQuit)
        {
            Exit(0);
        }
    }

    bool AmeEngine::IsRunning() const
    {
        return !m_ExitCode.has_value();
    }

    void AmeEngine::Exit(int exitCode)
    {
        m_ExitCode = exitCode;
    }

    //

    IModuleRegistry* AmeEngine::GetRegistry() const noexcept
    {
        return m_ModuleRegistery.get();
    }
} // namespace Ame
