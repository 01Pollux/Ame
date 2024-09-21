using AmeSharp.Core.Plugin;

namespace AmeSharp.Core.Config;

public class EngineConfig
{
    public CoreConfig Core = new();
    public RhiConfig? Rhi;

    public void RegisterInterface(IModuleRegistry registry, IPlugin? owner = null)
    {
        Core.RegisterInterface(registry, owner);
        Rhi?.RegisterInterface(registry, owner);
    }
}
