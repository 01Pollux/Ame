#include <Plugin/PluginContext.hpp>
#include <FileSystem/Utils.hpp>

namespace Ame
{
    namespace bdll         = boost::dll;
    using PluginCreateFunc = IPlugin* (*)();

    PluginContext::PluginContext(const String& pluginPath)
    {
        // sanitize plugin path
        if (!FileSystem::EnsurePathIsForward(pluginPath))
        {
            throw std::runtime_error("Invalid plugin path");
        }

        m_Library = LibraryDLL(boost::filesystem::path(PluginsPath) / pluginPath, bdll::load_mode::append_decorations);

        auto& createPlugin = m_Library.get<PluginCreateFunc>(AME_PLUGIN_EXPORT_NAME);

        m_Plugin.reset(createPlugin());
        m_Plugin->m_PluginName = pluginPath;
    }
} // namespace Ame