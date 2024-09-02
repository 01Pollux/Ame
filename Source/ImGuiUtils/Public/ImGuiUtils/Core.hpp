#pragma once

#include <span>
#include <imgui.h>

namespace Ame::ImGuiUtils
{
    /// <summary>
    /// Essentiel for ImGui to work properly across multiple dlls.
    /// </summary>
    void InitializeAllocators();

    /// <summary>
    /// Initialize ImGui allocators and set the current context.
    /// Essentiel for ImGui to work properly across multiple dlls.
    /// </summary>
    void InitializeAllocatorsAndContext(ImGuiContext* context);
} // namespace Ame::ImGuiUtils