#pragma once

namespace Ame::Log
{
    enum class LogLevel : unsigned char
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
        inline constexpr const char* c_Engine = "Engine";
        inline constexpr const char* c_Ecs    = "Ecs";
        inline constexpr const char* c_Client = "Client";
        inline constexpr const char* c_Editor = "Editor";
        inline constexpr const char* c_Asset  = "Asset";
        inline constexpr const char* c_Window = "Window";
        inline constexpr const char* c_Rhi    = "RHI";
        inline constexpr const char* c_Gfx    = "Gfx";
    } // namespace Names
} // namespace Ame::Log
