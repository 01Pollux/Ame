#include <Engine/Engine.hpp>

#include <Interfaces/Core/FrameTimer.hpp>
#include <Interfaces/Core/FrameEvent.hpp>
#include <Interfaces/Ecs/EntityStorage.hpp>
#include <Interfaces/Graphics/Renderer.hpp>

namespace Ame
{
    AmeEngine::AmeEngine(const EngineConfig& engineConfig) : m_ModuleRegistery(CreateModuleRegistry())
    {
        engineConfig.ExposeInterfaces(m_ModuleRegistery.get(), nullptr);
        RefreshSubmoduleCache();
    }

    AmeEngine::~AmeEngine() = default;

    //

    void AmeEngine::RefreshSubmoduleCache()
    {
        m_ModuleRegistery->RequestInterface(Interfaces::IID_FrameTimer, m_FrameTimer.DblPtr<IObject>());
        m_ModuleRegistery->RequestInterface(Interfaces::IID_FrameEvent, m_FrameEvent.DblPtr<IObject>());
        m_ModuleRegistery->RequestInterface(Interfaces::IID_EntityStorage, m_EntityStorage.DblPtr<IObject>());
        m_ModuleRegistery->RequestInterface(Interfaces::IID_Renderer, m_Renderer.DblPtr<IObject>());
    }

    //

    void AmeEngine::Tick()
    {
        bool shouldQuit = false;

        m_FrameTimer->Tick();

        m_FrameEvent->Invoke_OnFrameStart();
        m_FrameEvent->Invoke_OnFrameUpdate();

        if (m_EntityStorage)
        {
            shouldQuit |= !m_EntityStorage->Tick(m_FrameTimer->GetDeltaTime());
        }

        if (m_Renderer)
        {
            shouldQuit |= !m_Renderer->Tick();
        }

        shouldQuit |= !m_FrameEvent->Invoke_OnFrameEnd().value_or(true);

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
