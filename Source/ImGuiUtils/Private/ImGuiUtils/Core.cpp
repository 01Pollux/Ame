#include <Core/Allocator.hpp>
#include <ImGuiUtils/Core.hpp>

namespace Ame::ImGuiUtils
{
    void InitializeAllocators()
    {
        // MUST be identicial to ImGui_InitializeAllocators
        ImGui::SetAllocatorFunctions(
            [](size_t size, void*) { return mi_malloc(size); }, [](void* ptr, void*) { mi_free(ptr); });
    }

    void InitializeAllocatorsAndContext(ImGuiContext* context)
    {
        InitializeAllocators();
        ImGui::SetCurrentContext(context);
    }
} // namespace Ame::ImGuiUtils