using AmeSharp.Core.Plugin;

namespace AmeSharp.Core.Config;

public class ApplicationConfig
{
    public IModuleConfig[] Configs = [];

    public void ExposeInterfaces(IModuleRegistry registry, IPlugin? owner = null)
    {
        foreach (var config in Configs)
        {
            config.ExposeInterface(registry, owner);
        }
    }
}
