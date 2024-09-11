#pragma once

#include <Core/Interface.hpp>
#include <Window/Window.hpp>

namespace Ame::Interfaces
{
    inline constexpr UId IID_SurfaceWindow = Window::IID_Window;
    using ISurfaceWindow                   = Window::IWindow;
} // namespace Ame::Interfaces