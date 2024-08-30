#include <Plugin/PluginContext.hpp>

namespace Ame
{
    namespace bdll = boost::dll;

    PluginContext::PluginContext(const String& pluginPath)
    {
        // sanitize plugin path
        if (pluginPath.contains(".."))
        {
            throw std::runtime_error("Invalid plugin path");
        }

        m_Library = LibraryDLL(boost::filesystem::path(PluginsPath) / pluginPath, bdll::load_mode::append_decorations);

        using PluginCreateFunc = IPlugin* (*)();

        auto createPlugin = m_Library.get<PluginCreateFunc>(AME_PLUGIN_EXPORT_NAME);
        if (!createPlugin)
        {
            throw std::runtime_error("Plugin entry point not found");
        }

        m_Plugin.reset(createPlugin());
        m_Plugin->m_PluginName = pluginPath;
    }
} // namespace Ame