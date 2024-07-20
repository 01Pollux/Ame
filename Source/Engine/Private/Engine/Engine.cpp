#include <Engine/Engine.hpp>

#include <Module/Core/CoreModule.hpp>
#include <Module/Rhi/RhiModule.hpp>
#include <Module/Graphics/GraphicsModule.hpp>

#include <Log/Wrapper.hpp>

namespace Ame
{
    AmeEngine::AmeEngine(
        const EngineConfig& engineConfig)
    {
        m_ModuleRegistery.RegisterModule<CoreModule>(engineConfig.CoreConfig);
        if (engineConfig.RhiConfig)
        {
            auto rhiModule = m_ModuleRegistery.RegisterModule<RhiModule>(*engineConfig.RhiConfig);
            if (engineConfig.GraphicsConfig)
            {
                m_ModuleRegistery.RegisterModule<GraphicsModule>(rhiModule, *engineConfig.GraphicsConfig);
            }
        }

        RefreshSubmoduleCache();
    }

    AmeEngine::~AmeEngine() = default;

    //

    void AmeEngine::RefreshSubmoduleCache()
    {
        auto coreModule = m_ModuleRegistery.GetModule(IID_CoreModule);
        Log::Engine().Assert(coreModule, "Core module is not registered.");

        coreModule->QueryInterface(IID_TimeSubmodule, m_TimeSubmodule.DblPtr<IObject>());
        coreModule->QueryInterface(IID_FrameEventSubmodule, m_FrameEventSubmodule.DblPtr<IObject>());

        //

        auto graphicsModule = m_ModuleRegistery.GetModule(IID_GraphicsModule);
        if (graphicsModule)
        {
            graphicsModule->QueryInterface(IID_RendererSubmodule, m_RendererSubmodule.DblPtr<IObject>());
        }
    }

    //

    void AmeEngine::Tick()
    {
        bool shouldQuit = false;

        m_TimeSubmodule->GetTimer().Tick();

        m_FrameEventSubmodule->Invoke_OnFrameStart();
        m_FrameEventSubmodule->Invoke_OnFrameUpdate();

        if (m_RendererSubmodule)
        {
            shouldQuit |= !m_RendererSubmodule->Tick();
        }

        shouldQuit |= !m_FrameEventSubmodule->Invoke_OnFrameEnd().value_or(true);

        if (shouldQuit)
        {
            Exit(0);
        }
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
} // namespace Ame
