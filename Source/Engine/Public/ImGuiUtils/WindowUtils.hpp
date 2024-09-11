#pragma once

#include <ImGuiUtils/imcxx/all_in_one.hpp>

struct ImGuiWindow;

namespace Ame::ImGuiUtils
{
    /// <summary>
    /// Renders the outer borders of the window.
    /// </summary>
    AME_ENGINE_API void RenderWindowOuterBorders(ImGuiWindow* Window);
} // namespace Ame::ImGuiUtils