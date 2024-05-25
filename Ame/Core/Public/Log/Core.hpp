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
        static constexpr const char* c_Engine = "Engine";
        static constexpr const char* c_Ecs    = "Ecs";
        static constexpr const char* c_Client = "Client";
        static constexpr const char* c_Editor = "Editor";
        static constexpr const char* c_Asset  = "Asset";
        static constexpr const char* c_Window = "Window";
        static constexpr const char* c_Rhi    = "RHI";
        static constexpr const char* c_Gfx    = "Gfx";
    } // namespace Names
} // namespace Ame::Log
