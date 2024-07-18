#pragma once

#include <Core/Interface.hpp>
#include <Core/Signal.hpp>
#include <Math/Size2.hpp>

namespace Ame::Signals
{
    AME_SIGNAL_DECL(OnWindowSizeChanged, void(const Math::Size2I& newSize));
    AME_SIGNAL_DECL(OnWindowMinized, void(bool minimized));
    AME_SIGNAL_DECL(OnWindowClosed, void());
    AME_SIGNAL_DECL(OnWindowTitleHitTest, bool(const Math::Vector2I& mousePos));
} // namespace Ame::Signals

namespace Ame::Window
{
    class WindowEventListener
    {
    public:
        /// <summary>
        /// Event fired when the window is resized
        /// </summary>
        AME_SIGNAL_INST(OnWindowSizeChanged);

        /// <summary>
        /// Event fired when the window is minimized or restored
        /// </summary>
        AME_SIGNAL_INST(OnWindowMinized);

        /// <summary>
        /// Event fired when the window is closed
        /// </summary>
        AME_SIGNAL_INST(OnWindowClosed);

        /// <summary>
        /// Event fired when the window title bar is hit
        /// </summary>
        AME_SIGNAL_INST(OnWindowTitleHitTest);
    };
} // namespace Ame::Window
