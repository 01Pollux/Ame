#pragma once

#include <Core/Ame.hpp>
#include <Core/String.hpp>
#include <Math/Size2.hpp>

namespace Ame::Windowing
{
    class GlfwContext;

    struct WindowDesc
    {
        /// <summary>
        /// Previous context to have single glfw context across processes for the engine
        /// </summary>
        Ptr<GlfwContext> PreviousContext;

        /// <summary>
        /// Window title
        /// </summary>
        const char* Title = nullptr;

        /// <summary>
        /// Window size
        /// </summary>
        Math::Size2I Size = { 800, 600 };

        /// <summary>
        /// Apply custom title bar
        /// </summary>
        bool CustomTitleBar : 1 = false;

        /// <summary>
        /// Window will start in the middle of the screen
        /// </summary>
        bool StartInMiddle : 1 = true;

        /// <summary>
        /// Window will start in full screen mode
        /// </summary>
        bool FullScreen : 1 = false;

        /// <summary>
        /// Window will start maximized
        /// </summary>
        bool Maximized : 1 = false;

        /// <summary>
        /// Window will not be resizable
        /// </summary>
        bool NoResize : 1 = false;
    };
} // namespace Ame::Windowing