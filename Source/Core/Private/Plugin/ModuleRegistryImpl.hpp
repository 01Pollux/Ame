#pragma once

#include <map>
#include <Plugin/ModuleRegistry.hpp>
#include <Plugin/PluginContext.hpp>
#include <Plugin/InterfaceContext.hpp>

namespace Ame
{
    class ModuleRegistryImpl : public IModuleRegistry
    {
        using InterfaceMap = std::map<UId, InterfaceContext, UIdUtils::Comparer>;
        using PluginMap    = std::map<String, UniquePtr<PluginContext>>;

    public:
        ~ModuleRegistryImpl() override;

    public:
        bool ExposeInterface(IPlugin* owner, const UId& iid, IObject* object) override;
        bool RequestInterface(IPlugin* caller, const UId& iid, IObject** iface) override;
        bool DropInterface(const UId& iid);

    public:
        IPlugin* FindPlugin(const String& name) override;
        IPlugin* BindPlugin(IPlugin* caller, const String& name, bool isRequired) override;

    public:
        void     Shutdown() override;
        TVersion GetHostVersion() override;

    public:
        IPlugin* LoadPlugin(const String& name) override;
        bool     UnloadPlugin(const String& name) override;

    private:
        [[nodiscard]] auto FindContext(const String& name) noexcept
        {
            auto iter = m_Plugins.find(name);
            return iter != m_Plugins.end() ? iter->second.get() : nullptr;
        }

        [[nodiscard]] auto FindContext(const IPlugin* plugin) noexcept
        {
            return FindContext(plugin->GetPluginName());
        }

    private:
        void ReleaseAllPlugins();
        void UnloadPlugin_Internal(PluginContext& context);

    private:
        InterfaceMap m_Interfaces;
        PluginMap    m_Plugins;
    };
} // namespace Ame