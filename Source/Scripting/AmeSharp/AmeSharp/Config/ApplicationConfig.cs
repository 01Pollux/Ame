using AmeSharp.Plugin;

namespace AmeSharp.Config;

public class ApplicationConfig
{
    public EngineConfig Engine = new();

    public void RegisterInterface(IModuleRegistry registry, IPlugin? owner = null)
    {
        Engine.RegisterInterface(registry, owner);
    }
}
