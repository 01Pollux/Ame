#include <Engine/Engine.hpp>

#include <Module/Core/CoreModule.hpp>
#include <Module/Graphics/GraphicsModule.hpp>

namespace Ame
{
    AmeEngine::AmeEngine(
        const EngineConfig& engineConfig)
    {
        m_ModuleRegistery.RegisterModule<CoreModule>(engineConfig.CoreConfig);
        if (engineConfig.GraphicsConfig)
        {
            m_ModuleRegistery.RegisterModule<GraphicsModule>(*engineConfig.GraphicsConfig);
        }

        PreloadSubmodules();
    }

    AmeEngine::~AmeEngine() = default;

    //

    void AmeEngine::Tick()
    {
        m_TimeSubmodule->GetTimer().Tick();
        m_FrameEventSubmodule->Invoke_OnFrameStart();
        m_FrameEventSubmodule->Invoke_OnFrameUpdate();
        m_FrameEventSubmodule->Invoke_OnFrameEnd();
    }

    bool AmeEngine::IsRunning() const
    {
        return !m_ExitCode.has_value();
    }

    void AmeEngine::Exit(
        int exitCode)
    {
        m_ExitCode = exitCode;
    }

    //

    const ModuleRegistry& AmeEngine::GetRegistry() const noexcept
    {
        return m_ModuleRegistery;
    }

    ModuleRegistry& AmeEngine::GetRegistry() noexcept
    {
        return m_ModuleRegistery;
    }

    //

    void AmeEngine::PreloadSubmodules()
    {
        auto coreModule = m_ModuleRegistery.GetModule(IID_CoreModule);

        coreModule->QueryInterface(IID_TimeSubmodule, m_TimeSubmodule.DblPtr<IObject>());
        coreModule->QueryInterface(IID_FrameEventSubmodule, m_FrameEventSubmodule.DblPtr<IObject>());
    }
} // namespace Ame
