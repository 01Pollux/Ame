#pragma once

#include <map>
#include <boost/property_tree/ptree.hpp>
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
        using PluginConfigDesc = boost::property_tree::ptree;
        using InterfaceMap     = std::map<UId, InterfaceDesc, UIdUtils::Comparer>;
        using PluginMap        = std::map<UId, UniquePtr<PluginContext>, UIdUtils::Comparer>;

    public:
        PluginHostImpl();

    public:
        bool     ExposeInterface(const UId& iid, IObject* object, IPlugin* owner) override;
        bool     RequestInterface(const UId& iid, IObject** iface) override;
        IPlugin* FindPlugin(const UId& iid) override;
        IPlugin* BindPlugin(IPlugin* caller, const UId& iid, bool isRequired) override;
        void     Shutdown() override;
        TVersion GetHostVersion() override;
        IPlugin* LoadPlugin(const UId& iid) override;
        bool     UnloadPlugin(const UId& iid) override;

    private:
        [[nodiscard]] auto FindContext(const UId& iid) noexcept
        {
            auto iter = m_Plugins.find(iid);
            return iter != m_Plugins.end() ? iter->second.get() : nullptr;
        }

        [[nodiscard]] auto FindContext(const IPlugin* plugin) noexcept
        {
            return FindContext(plugin->GetPluginInfo().Id);
        }

    private:
        void UnloadPlugin_Internal(PluginContext& context);

    private:
        PluginConfigDesc m_PluginConfig;
        InterfaceMap     m_Interfaces;
        PluginMap        m_Plugins;
    };
} // namespace Ame