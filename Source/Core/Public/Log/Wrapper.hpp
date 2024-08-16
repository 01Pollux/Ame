#pragma once

#include <Log/Logger.hpp>

namespace Ame::Log
{
    /// <summary>
    /// Get the engine's logger
    /// </summary>
    [[nodiscard]] inline Logger& Engine()
    {
        return Logger::GetLogger(Names::c_Engine);
    }

    /// <summary>
    /// Get the ecs's logger
    /// </summary>
    [[nodiscard]] inline Logger& Ecs()
    {
        return Logger::GetLogger(Names::c_Ecs);
    }

    /// <summary>
    /// Get the client's logger
    /// </summary>
    [[nodiscard]] inline Logger& Client()
    {
        return Logger::GetLogger(Names::c_Client);
    }

    /// <summary>
    /// Get the editor's logger
    /// </summary>
    [[nodiscard]] inline Logger& Editor()
    {
        return Logger::GetLogger(Names::c_Editor);
    }

    /// <summary>
    /// Get the asset's logger
    /// </summary>
    [[nodiscard]] inline Logger& Asset()
    {
        return Logger::GetLogger(Names::c_Asset);
    }

    /// <summary>
    /// Get the window's logger
    /// </summary>
    [[nodiscard]] inline Logger& Window()
    {
        return Logger::GetLogger(Names::c_Window);
    }

    /// <summary>
    /// Get the rhi's logger
    /// </summary>
    [[nodiscard]] inline Logger& Rhi()
    {
        return Logger::GetLogger(Names::c_Rhi);
    }

    /// <summary>
    /// Get the gfx's logger
    /// </summary>
    [[nodiscard]] inline Logger& Gfx()
    {
        return Logger::GetLogger(Names::c_Gfx);
    }
} // namespace Ame::Log
