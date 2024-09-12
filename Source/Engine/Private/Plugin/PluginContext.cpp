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

        auto path = boost::filesystem::path(PluginsPath) / pluginPath;
        m_Library = LibraryDLL(path, bdll::load_mode::append_decorations);

        auto& createPlugin = m_Library.get<PluginCreateFunc>(AME_PLUGIN_EXPORT_NAME);

        m_Plugin.reset(createPlugin());
        m_Plugin->m_PluginName = path.filename().string();
        m_Plugin->m_PluginPath = path.parent_path().string();
    }
} // namespace Ame