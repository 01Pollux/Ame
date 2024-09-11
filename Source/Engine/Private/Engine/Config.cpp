#include <Engine/Config.hpp>

namespace Ame
{
    void EngineConfig::ExposeInterfaces(IModuleRegistry* registry, IPlugin* owner) const
    {
        CoreConfig.ExposeInterfaces(registry, owner);
        if (RhiConfig)
        {
            RhiConfig->ExposeInterfaces(registry, owner);
        }
        if (EcsConfig)
        {
            EcsConfig->ExposeInterfaces(registry, owner);
        }
        if (GraphicsConfig)
        {
            GraphicsConfig->ExposeInterfaces(registry, owner);
        }
    }
} // namespace Ame
