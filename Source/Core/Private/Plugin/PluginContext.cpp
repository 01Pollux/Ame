#include <Plugin/PluginContext.hpp>

namespace Ame
{
    namespace bdll = boost::dll;

    PluginContext::PluginContext(const String& pluginPath, const UId& iid)
    {
        if (pluginPath.empty())
        {
            m_Library = LibraryDLL(bdll::program_location());
        }
        else
        {
            m_Library = LibraryDLL(pluginPath, bdll::load_mode::append_decorations);
        }

        using PluginCreateFunc = IPlugin* (*)();

        auto createPlugin = m_Library.get<PluginCreateFunc>(AME_PLUGIN_EXPORT_NAME);
        if (!createPlugin)
        {
            throw std::runtime_error("Plugin entry point not found");
        }

        m_Plugin.reset(createPlugin());
        if (m_Plugin->GetPluginInfo().Id != iid)
        {
            throw std::runtime_error("Plugin ID mismatch");
        }
    }
} // namespace Ame