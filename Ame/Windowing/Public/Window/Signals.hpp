#pragma once

#include <Object/Signal.hpp>
#include <Math/Size2.hpp>

namespace Ame::Windowing
{
    class Window;
} // namespace Ame::Windowing

AME_SIGNAL_DECL(OnWindowSizeChanged, void(const Ame::Math::Size2I& newSize));

AME_SIGNAL_DECL(OnWindowMinized, void(bool minimized));

AME_SIGNAL_DECL(OnWindowClosed, void());

AME_SIGNAL_DECL(OnWindowTitleHitTest, bool(const Ame::Math::Vector2I& mousePos));
