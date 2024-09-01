#include <Interfaces/Graphics/Config.hpp>
#include <Interfaces/Graphics/Renderer.hpp>

#include <Interfaces/Rhi/RhiDevice.hpp>
#include <Interfaces/Ecs/EntityWorld.hpp>
#include <Interfaces/Rhi/ImGuiRenderer.hpp>

namespace Ame::Interfaces
{
    void GraphicsModuleConfig::ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner) const
    {
        Ptr<IRhiDevice>     rhiDevice;
        Ptr<IEntityWorld>   entityWorld;
        Ptr<IImGuiRenderer> imguiRenderer;

        registry->RequestInterface(owner, IID_RhiDevice, rhiDevice.DblPtr<IObject>());
        registry->RequestInterface(owner, IID_EntityWorld, entityWorld.DblPtr<IObject>());
        registry->RequestInterface(owner, IID_ImGuiRenderer, imguiRenderer.DblPtr<IObject>());

        RendererCreateDesc createDesc{ rhiDevice, entityWorld, imguiRenderer };

        registry->ExposeInterface(owner, IID_Renderer, CreateRenderer(createDesc));
    }
} // namespace Ame::Interfaces