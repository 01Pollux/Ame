#pragma once

#include <ImGuiUtils/imcxx/all_in_one.hpp>

struct ImGuiWindow;

namespace Ame::ImGuiUtils
{
    /// <summary>
    /// Renders the outer borders of the window.
    /// </summary>
    void RenderWindowOuterBorders(ImGuiWindow* Window);
} // namespace Ame::ImGuiUtils