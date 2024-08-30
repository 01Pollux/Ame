#pragma once

#include <map>
#include <Plugin/PluginHost.hpp>
#include <Plugin/PluginContext.hpp>

namespace Ame
{
    struct InterfaceDesc
    {
        Ptr<IObject> Object;
        IPlugin*     Owner;
    };

    class PluginHostImpl : public IPluginHost
    {
        using InterfaceMap = std::map<UId, InterfaceDesc, UIdUtils::Comparer>;
        using PluginMap    = std::map<String, UniquePtr<PluginContext>>;

    public:
        ~PluginHostImpl() override;
        bool     ExposeInterface(const UId& iid, IObject* object, IPlugin* owner) override;
        bool     RequestInterface(const UId& iid, IObject** iface) override;
        IPlugin* FindPlugin(const String& name) override;
        IPlugin* BindPlugin(IPlugin* caller, const String& name, bool isRequired) override;
        void     Shutdown() override;
        TVersion GetHostVersion() override;
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