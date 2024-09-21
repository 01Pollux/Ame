using AmeSharp.Core.Plugin;

namespace AmeSharp.Core.Config;

public class ApplicationConfig
{
    public EngineConfig Engine = new();

    public void RegisterInterface(IModuleRegistry registry, IPlugin? owner = null)
    {
        Engine.RegisterInterface(registry, owner);
    }
}
