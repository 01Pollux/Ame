#pragma once

#include <Core/Ame.hpp>
#include <Module/ModuleRegistry.hpp>

namespace Ame
{
    class TimeSubmodule;

    class AmeEngine
    {
    public:
        AmeEngine();

    public:
        void               Tick();
        [[nodiscard]] bool IsRunning() const;
        void               Exit(int exitCode = 0);

    public:
        [[nodiscard]] const ModuleRegistry& GetRegistry() const noexcept;
        [[nodiscard]] const ModuleRegistry& GetRegistry() noexcept;

    private:
        void PreloadSubmodules();

    private:
        ModuleRegistry m_ModuleRegistery;

        // precaching frequently used submodules
        Ptr<TimeSubmodule> m_TimeSubmodule;

        Opt<int> m_ExitCode;
    };
} // namespace Ame
