#pragma once

#include <Core/Interface.hpp>
#include <Math/Size2.hpp>

namespace Ame::Window
{
    class IWindow;

    //

    enum class WindowType : uint8_t
    {
        // Uses GLFW backend for window creation
        DesktopWindow,
        Count
    };

    struct WindowCreateDesc
    {
        /// <summary>
        /// Window title
        /// </summary>
        const char* Title = nullptr;

        WindowType Type = WindowType::DesktopWindow;

        /// <summary>
        /// Window size
        /// </summary>
        Math::Size2I Size{ 800, 600 };

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

    //

    AME_DECL_CLASS(WindowFactory, "{35e69e93-2904-4182-a85e-baf623c86752}");

    AME_INTERFACE_BEGIN("{d4c5a174-763c-4e6b-83f8-73355002fb7e}", IWindowFactory);
    [[nodiscard]] virtual Ptr<IWindow> CreateWindow(const WindowCreateDesc& createDesc) = 0;
    AME_INTERFACE_END();
} // namespace Ame::Window
