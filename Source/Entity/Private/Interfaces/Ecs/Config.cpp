#include <Interfaces/Ecs/Config.hpp>
#include <Interfaces/Ecs/EntityWorld.hpp>
#include <Interfaces/Rhi/RhiDevice.hpp>

namespace Ame::Interfaces
{
    void EcsModuleConfig::ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner) const
    {
        Ptr<IRhiDevice> rhiDevice;
        registry->RequestInterface(owner, IID_RhiDevice, rhiDevice.DblPtr<IObject>());
        registry->ExposeInterface(owner, IID_EntityWorld, CreateEntityWorld(rhiDevice, WorldDesc));
    }
} // namespace Ame::Interfaces
