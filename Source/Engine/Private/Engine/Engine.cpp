#include <Engine/Engine.hpp>

#include <Module/Core/CoreModule.hpp>

namespace Ame
{
    AmeEngine::AmeEngine()
    {
        m_ModuleRegistery.RegisterModule<CoreModule>();

        PreloadSubmodules();
    }

    //

    void AmeEngine::Tick()
    {
        m_TimeSubmodule->GetTimer().Tick();
    }

    bool AmeEngine::IsRunning() const
    {
        return m_ExitCode.has_value();
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

    const ModuleRegistry& AmeEngine::GetRegistry() noexcept
    {
        return m_ModuleRegistery;
    }

    //

    void AmeEngine::PreloadSubmodules()
    {
        auto coreModule = m_ModuleRegistery.GetModule(IID_CoreModule);

        coreModule->QueryInterface(IID_TimeSubmodule, m_TimeSubmodule.DblPtr<IObject>());
    }
} // namespace Ame
