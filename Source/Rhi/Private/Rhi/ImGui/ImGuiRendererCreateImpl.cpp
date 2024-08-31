#include <Rhi/ImGui/ImGuiNullRendererImpl.hpp>
#include <Rhi/ImGui/ImGuiDiligentRendererImpl.hpp>

namespace Ame::Rhi
{
    Ptr<IImGuiRenderer> CreateImGuiRenderer(const ImGuiRendererCreateDesc& desc)
    {
#ifdef AME_NO_IMGUI
        return CreateImGuiNullRenderer();
#else
        return { ObjectAllocator<ImGuiDiligentRendererImpl>()(desc), IID_ImGuiRenderer };
#endif
    }

    Ptr<IImGuiRenderer> CreateImGuiNullRenderer()
    {
        return { ObjectAllocator<ImGuiNullRendererImpl>()(), IID_ImGuiRenderer };
    }
} // namespace Ame::Rhi
