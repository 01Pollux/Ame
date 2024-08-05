#include <Module/Graphics/RendererSubmodule.hpp>

#include <Module/Rhi/RhiModule.hpp>
#include <Rhi/ImGui/ImGuiRenderer.hpp>
#include <Module/Ecs/EntityModule.hpp>

#include <imgui.h>

namespace Ame
{
    [[nodiscard]]
    static Ptr<Rhi::IRhiDevice> GetRhiDevice(
        RhiModule* rhiModule)
    {
        Ptr<Rhi::IRhiDevice> rhiDevice;
        rhiModule->QueryInterface(Rhi::IID_RhiDevice, rhiDevice.DblPtr<IObject>());
        return rhiDevice;
    }

    [[nodiscard]]
    static Ptr<Rhi::IImGuiRenderer> GetImGuiRenderer(
        RhiModule* rhiModule)
    {
        Ptr<Rhi::IImGuiRenderer> imguiRenderer;
#ifndef AME_NO_IMGUI
        rhiModule->QueryInterface(Rhi::IID_ImGuiRenderer, imguiRenderer.DblPtr<IObject>());
#endif
        return imguiRenderer;
    }

    [[nodiscard]] static Ptr<Ecs::World> GetWorld(
        EntityModule* entityModule)
    {
        Ptr<Ecs::World> world;
        entityModule->QueryInterface(Ecs::IID_EntityWorld, world.DblPtr<IObject>());
        return world;
    }

    RendererSubmodule::RendererSubmodule(
        IReferenceCounters*         counters,
        RhiModule*                  rhiModule,
        EntityModule*               entityModule,
        const GraphicsModuleConfig& config) :
        Base(counters, IID_RendererSubmodule),
        m_Renderer(ObjectAllocator<Gfx::Renderer>()(GetRhiDevice(rhiModule), GetWorld(entityModule), GetImGuiRenderer(rhiModule)))
    {
    }

    bool RendererSubmodule::Tick()
    {
        return m_Renderer->Tick();
    }
} // namespace Ame