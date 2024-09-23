using AmeSharp.Core.Plugin;

namespace AmeSharp.Core.Config;

public interface IModuleConfig
{
    void ExposeInterface(IModuleRegistry registry, IPlugin? owner = null);
}
