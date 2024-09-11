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
        Signals::OnWindowSizeChanged_Signal OnWindowSizeChanged;

        /// <summary>
        /// Event fired when the window is minimized or restored
        /// </summary>
        Signals::OnWindowMinized_Signal OnWindowMinized;

        /// <summary>
        /// Event fired when the window is closed
        /// </summary>
        Signals::OnWindowClosed_Signal OnWindowClosed;

        /// <summary>
        /// Event fired when the window title bar is hit
        /// </summary>
        Signals::OnWindowTitleHitTest_Signal OnWindowTitleHitTest;
    };
} // namespace Ame::Window
