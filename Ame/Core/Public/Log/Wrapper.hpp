#pragma once

#include <Log/Logger.hpp>

namespace Ame::Log
{
    /// <summary>
    /// Get the engine's logger
    /// </summary>
    static Logger& Engine()
    {
        return Logger::GetLogger(Names::c_Engine);
    }

    /// <summary>
    /// Get the ecs's logger
    /// </summary>
    static Logger& Ecs()
    {
        return Logger::GetLogger(Names::c_Ecs);
    }

    /// <summary>
    /// Get the client's logger
    /// </summary>
    [[nodiscard]] static Logger& Client()
    {
        return Logger::GetLogger(Names::c_Client);
    }

    /// <summary>
    /// Get the editor's logger
    /// </summary>
    static Logger& Editor()
    {
        return Logger::GetLogger(Names::c_Editor);
    }

    /// <summary>
    /// Get the asset's logger
    /// </summary>
    [[nodiscard]] static Logger& Asset()
    {
        return Logger::GetLogger(Names::c_Asset);
    }

    /// <summary>
    /// Get the window's logger
    /// </summary>
    [[nodiscard]] static Logger& Window()
    {
        return Logger::GetLogger(Names::c_Window);
    }

    /// <summary>
    /// Get the rhi's logger
    /// </summary>
    [[nodiscard]] static Logger& Rhi()
    {
        return Logger::GetLogger(Names::c_Rhi);
    }

    /// <summary>
    /// Get the gfx's logger
    /// </summary>
    [[nodiscard]] static Logger& Gfx()
    {
        return Logger::GetLogger(Names::c_Gfx);
    }
} // namespace Ame::Log
