#pragma once

#include <Core/String.hpp>

namespace Ame::Log
{
    enum class LogLevel : uint8_t
    {
        Disabled,
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

    class Logger;

    namespace Names
    {
        static constexpr const char* Engine = "Engine";
        static constexpr const char* Ecs    = "Ecs";
        static constexpr const char* Client = "Client";
        static constexpr const char* Editor = "Editor";
        static constexpr const char* Asset  = "Asset";
        static constexpr const char* Window = "Window";
        static constexpr const char* Rhi    = "RHI";
        static constexpr const char* Gfx    = "Gfx";
    } // namespace Names
} // namespace Ame::Log
