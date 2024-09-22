#pragma once

#include <Core/Interface.hpp>

namespace Ame::Window
{
    class IWindow;

    //

    enum class WindowType : uint8_t
    {
        DesktopWindow,
        // WebWindow,
        Count
    };

    struct WindowCreateDesc
    {
        /// <summary>
        /// Window title
        /// </summary>
        const char* Title = nullptr;

        uint32_t Width  = 800;
        uint32_t Height = 600;

        /// <summary>
        /// Apply custom title bar
        /// </summary>
        bool CustomTitleBar;

        /// <summary>
        /// Window will start in the middle of the screen
        /// </summary>
        bool StartInMiddle;

        /// <summary>
        /// Window will start in full screen mode
        /// </summary>
        bool FullScreen;

        /// <summary>
        /// Window will start maximized
        /// </summary>
        bool Maximized;

        /// <summary>
        /// Window will not be resizable
        /// </summary>
        bool NoResize;
    };
} // namespace Ame::Window
