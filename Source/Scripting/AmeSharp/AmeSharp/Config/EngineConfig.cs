using AmeSharp.Plugin;

namespace AmeSharp.Config;

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
