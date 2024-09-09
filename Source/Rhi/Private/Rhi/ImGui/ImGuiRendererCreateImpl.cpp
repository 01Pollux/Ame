#include <Rhi/ImGui/ImGuiNullRendererImpl.hpp>
#include <Rhi/ImGui/ImGuiDiligentRendererImpl.hpp>

namespace Ame::Rhi
{
    Ptr<IImGuiRenderer> CreateImGuiRenderer(const ImGuiRendererCreateDesc& desc)
    {
#ifdef AME_NO_IMGUI
        return CreateImGuiNullRenderer();
#else
        return AmeCreate(ImGuiDiligentRendererImpl, desc);
#endif
    }

    Ptr<IImGuiRenderer> CreateImGuiNullRenderer()
    {
        return AmeCreate(ImGuiNullRendererImpl);
    }
} // namespace Ame::Rhi
