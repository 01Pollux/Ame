#include <Module/Graphics/RendererSubmodule.hpp>

#include <Module/Rhi/RhiModule.hpp>
#include <Rhi/ImGui/ImGuiRenderer.hpp>

#include <imgui.h>

namespace Ame
{
    static Ptr<Rhi::IRhiDevice> GetRhiDevice(
        RhiModule* rhiModule)
    {
        Ptr<Rhi::IRhiDevice> rhiDevice;
        rhiModule->QueryInterface(Rhi::IID_RhiDevice, rhiDevice.DblPtr<IObject>());
        return rhiDevice;
    }

    static Ptr<Rhi::IImGuiRenderer> GetImGuiRenderer(
        RhiModule* rhiModule)
    {
        Ptr<Rhi::IImGuiRenderer> imguiRenderer;
#ifndef AME_NO_IMGUI
        rhiModule->QueryInterface(Rhi::IID_ImGuiRenderer, imguiRenderer.DblPtr<IObject>());
#endif
        return imguiRenderer;
    }

    RendererSubmodule::RendererSubmodule(
        IReferenceCounters*         counters,
        RhiModule*                  rhiModule,
        const GraphicsModuleConfig& config) :
        Base(counters, IID_RendererSubmodule),
        m_Renderer(ObjectAllocator<Graphics::Renderer>()(GetRhiDevice(rhiModule), GetImGuiRenderer(rhiModule)))
    {
    }

    bool RendererSubmodule::Tick()
    {
        return m_Renderer->Tick();
    }
} // namespace Ame