#include <Interfaces/Ecs/EntityStorage.hpp>
#include <Interfaces/Rhi/RhiBackend.hpp>

namespace Ame::Interfaces
{
    void EcsModuleConfig::ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner) const
    {
        Ptr<RhiBackend> rhiBackend;
        registry->RequestInterface(owner, IID_RhiBackend, rhiBackend.DblPtr<IObject>());
        registry->ExposeInterface(
            owner, IID_EntityStorage, AmeCreateObject(EntityStorage, rhiBackend->GetRhiDevice(), StorageDesc));
    }
} // namespace Ame::Interfaces
