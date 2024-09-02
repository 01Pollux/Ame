#include <Plugin/Plugin.hpp>
#include <Plugin/ModuleRegistry.hpp>
#include <Interfaces/Core/Logger.hpp>

namespace Ame
{
    IPlugin::IPlugin(const PluginInfo& info) noexcept : m_PluginInfo(info)
    {
    }

    bool IPlugin::OnPluginPreLoad(IModuleRegistry* registry)
    {
        registry->RequestInterface(this, Interfaces::IID_Logger, Log::s_Logger.DblPtr<IObject>());
        return true;
    }
} // namespace Ame