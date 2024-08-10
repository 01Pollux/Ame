#include <Engine/Engine.hpp>

#include <Module/Core/CoreModule.hpp>
#include <Module/Rhi/RhiModule.hpp>
#include <Module/Ecs/EntityModule.hpp>
#include <Module/Graphics/GraphicsModule.hpp>

#include <Log/Wrapper.hpp>

#include <iostream>

namespace Ame
{
    AmeEngine::AmeEngine(
        const EngineConfig& engineConfig)
    {
        m_ModuleRegistery.RegisterModule<CoreModule>(engineConfig.CoreConfig);
        if (engineConfig.RhiConfig)
        {
            m_ModuleRegistery.RegisterModule<RhiModule>(*engineConfig.RhiConfig);
        }

        if (engineConfig.EcsConfig)
        {
            m_ModuleRegistery.RegisterModule<EntityModule>(*engineConfig.EcsConfig);
        }

        if (engineConfig.GraphicsConfig)
        {
            GraphicsModule::Dependencies deps{
                m_ModuleRegistery.GetModule<RhiModule>(IID_RhiModule),
                m_ModuleRegistery.GetModule<EntityModule>(IID_EntityModule)
            };
            m_ModuleRegistery.RegisterModule<GraphicsModule>(deps, *engineConfig.GraphicsConfig);
        }

        RefreshSubmoduleCache();
    }

    AmeEngine::~AmeEngine() = default;

    //

    void AmeEngine::RefreshSubmoduleCache()
    {
        auto& coreModule = m_ModuleRegistery.GetModule(IID_CoreModule);

        coreModule->QueryInterface(IID_TimeSubmodule, m_TimeSubmodule.DblPtr<IObject>());
        coreModule->QueryInterface(IID_FrameEventSubmodule, m_FrameEventSubmodule.DblPtr<IObject>());

        //

        if (m_ModuleRegistery.ContainsModule(IID_GraphicsModule))
        {
            auto& graphicsModule = m_ModuleRegistery.GetModule(IID_GraphicsModule);
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
