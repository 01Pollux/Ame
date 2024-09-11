#pragma once

#include <Engine/Config.hpp>
#include <Plugin/ModuleRegistry.hpp>

#include <Interfaces/Ecs/EntityWorld.hpp>
#include <Interfaces/Graphics/Renderer.hpp>

namespace Ame
{
    class AME_ENGINE_API AmeEngine
    {
    public:
        explicit AmeEngine(const EngineConfig& engineConfig);

        AmeEngine(const AmeEngine&)            = delete;
        AmeEngine& operator=(const AmeEngine&) = delete;

        AmeEngine(AmeEngine&&)            = delete;
        AmeEngine& operator=(AmeEngine&&) = delete;

        ~AmeEngine();

    public:
        /// <summary>
        /// Refreshes the submodule cache if a module is added or removed.
        /// </summary>
        void RefreshSubmoduleCache();

    public:
        void               Tick();
        [[nodiscard]] bool IsRunning() const;
        void               Exit(int exitCode = 0);

    public:
        [[nodiscard]] IModuleRegistry* GetRegistry() const noexcept;

    private:
        UniquePtr<IModuleRegistry> m_ModuleRegistery;

        // precaching frequently used submodules
        Ptr<Interfaces::FrameTimer>   m_FrameTimer;
        Ptr<Interfaces::FrameEvent>   m_FrameEvent;
        Ptr<Interfaces::IEntityWorld> m_EntityWorld;
        Ptr<Interfaces::IRenderer>    m_Renderer;

        Opt<int> m_ExitCode;
    };
} // namespace Ame
