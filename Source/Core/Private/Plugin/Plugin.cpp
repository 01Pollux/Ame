#include <Plugin/Plugin.hpp>
#include <Plugin/ModuleRegistry.hpp>
#include <Interfaces/Core/Logger.hpp>

namespace Ame
{
    bool IPlugin::OnPluginPreLoad(IModuleRegistry* registry)
    {
        registry->RequestInterface(this, Interfaces::IID_Logger, Log::Logger.DblPtr<IObject>());
        return true;
    }
} // namespace Ame