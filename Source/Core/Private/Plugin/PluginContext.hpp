#pragma once

#include <filesystem>
#include <set>
#include <boost/dll.hpp>
#include <Plugin/Plugin.hpp>

namespace Ame
{
    class PluginContext
    {
        using LibraryDLL                              = boost::dll::shared_library;
        static constexpr const StringView PluginsPath = "Plugins";

    public:
        PluginContext(const String& pluginPath);
        ~PluginContext()
        {
            if (m_Plugin)
            {
                m_Plugin->OnPluginUnload();
            }
        }

        void Invalidate()
        {
            m_Plugin = nullptr;
        }

    public:
        [[nodiscard]] IPlugin* GetPlugin()
        {
            return m_Plugin.get();
        }

        [[nodiscard]] const PluginInfo& GetPluginInfo() const noexcept
        {
            return m_Plugin->GetPluginInfo();
        }

        void AddDependencies(IPlugin* plugin)
        {
            m_Dependencies.insert(plugin);
        }

        [[nodiscard]] bool HasDependencies() const noexcept
        {
            return !m_Dependencies.empty();
        }

        [[nodiscard]] auto& GetDependencies() const noexcept
        {
            return m_Dependencies;
        }

    private:
        LibraryDLL         m_Library;
        UniquePtr<IPlugin> m_Plugin = nullptr;
        std::set<IPlugin*> m_Dependencies;
    };
} // namespace Ame