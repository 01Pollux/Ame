#pragma once

#include <Object/Signal.hpp>
#include <Math/Size2.hpp>

namespace Ame::Windowing
{
    class Window;
} // namespace Ame::Windowing

AME_SIGNAL_INSTANCE_DECL(Ame::Windowing::Window, OnWindowSizeChanged, const Ame::Math::Size2I& /*newSize*/);

AME_SIGNAL_INSTANCE_DECL(Ame::Windowing::Window, OnWindowMinized, bool /*minimized*/);

AME_SIGNAL_INSTANCE_DECL(Ame::Windowing::Window, OnWindowClosed);

AME_SIGNAL_INSTANCE_DECL(Ame::Windowing::Window, OnWindowTitleHitTest, const Ame::Math::Vector2I& /*mousePos*/, bool& /*wasHit*/);
