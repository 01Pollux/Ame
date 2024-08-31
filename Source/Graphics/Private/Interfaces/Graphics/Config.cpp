#include <Interfaces/Graphics/Renderer.hpp>

#include <Interfaces/Rhi/RhiBackend.hpp>
#include <Interfaces/Ecs/EntityStorage.hpp>
#include <Interfaces/Rhi/ImGuiBackend.hpp>

namespace Ame::Interfaces
{
    void GraphicsModuleConfig::ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner) const
    {
        Ptr<RhiBackend>    rhibackend;
        Ptr<EntityStorage> entityStorage;
        Ptr<ImGuiBackend>  imguiBackend;

        registry->RequestInterface(owner, IID_RhiBackend, rhibackend.DblPtr<IObject>());
        registry->RequestInterface(owner, IID_EntityStorage, entityStorage.DblPtr<IObject>());
        registry->RequestInterface(owner, IID_ImGuiBackend, imguiBackend.DblPtr<IObject>());

        RendererCreateDesc createDesc{ rhibackend->GetRhiDevice(), entityStorage->GetWorld(),
                                       imguiBackend->GetImGuiRenderer() };

        registry->ExposeInterface(owner, IID_Renderer, CreateRenderer(createDesc));
    }
} // namespace Ame::Interfaces