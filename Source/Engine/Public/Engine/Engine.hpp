#pragma once

#include <Engine/Config.hpp>
#include <Module/ModuleRegistry.hpp>

namespace Ame
{
    class AmeEngine
    {
    public:
        explicit AmeEngine(
            const EngineConfig& engineConfig);

        AmeEngine(const AmeEngine&)            = delete;
        AmeEngine& operator=(const AmeEngine&) = delete;

        AmeEngine(AmeEngine&&)            = delete;
        AmeEngine& operator=(AmeEngine&&) = delete;

        ~AmeEngine();

    public:
        void               Tick();
        [[nodiscard]] bool IsRunning() const;
        void               Exit(int exitCode = 0);

    public:
        [[nodiscard]] const ModuleRegistry& GetRegistry() const noexcept;
        [[nodiscard]] ModuleRegistry&       GetRegistry() noexcept;

    private:
        void PreloadSubmodules();

    private:
        ModuleRegistry m_ModuleRegistery;

        // precaching frequently used submodules
        Ptr<TimeSubmodule>       m_TimeSubmodule;
        Ptr<FrameEventSubmodule> m_FrameEventSubmodule;

        Opt<int> m_ExitCode;
    };
} // namespace Ame
