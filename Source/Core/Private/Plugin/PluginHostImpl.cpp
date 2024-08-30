#include <Plugin/PluginHostImpl.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <Log/Wrapper.hpp>

namespace Ame
{
    PluginHostImpl::PluginHostImpl()
    {
        boost::property_tree::read_xml("pd.ame", m_PluginConfig);
    }

    bool PluginHostImpl::ExposeInterface(const UId& iid, IObject* object, IPlugin* owner)
    {
        auto iter = m_Interfaces.find(iid);
        if (iter != m_Interfaces.end())
        {
            return false;
        }

        m_Interfaces.emplace(iid, InterfaceDesc{ Ptr{ object }, owner });
        return true;
    }

    bool PluginHostImpl::RequestInterface(const UId& iid, IObject** iface)
    {
        auto iter = m_Interfaces.find(iid);
        if (iter == m_Interfaces.end())
        {
            return false;
        }
        *iface = iter->second.Object;
        (*iface)->AddRef();
        return true;
    }

    IPlugin* PluginHostImpl::FindPlugin(const UId& iid)
    {
        auto ctx = FindContext(iid);
        return ctx ? ctx->GetPlugin() : nullptr;
    }

    IPlugin* PluginHostImpl::BindPlugin(IPlugin* caller, const UId& iid, bool isRequired)
    {
        IPlugin* plugin = nullptr;

        auto callerCtx = FindContext(caller);
        if (auto ctx = FindContext(iid))
        {
            plugin = ctx->GetPlugin();
        }
        else if (isRequired)
        {
            plugin = LoadPlugin(iid);
        }

        if (plugin)
        {
            callerCtx->AddDependencies(plugin);
        }
        return plugin;
    }

    void PluginHostImpl::Shutdown()
    {
        for (auto& iface : m_Interfaces | std::views::values)
        {
            iface.Owner->OnDropInterface(iface.Object);
        }

        for (auto& ctx : m_Plugins | std::views::values)
        {
            ctx->GetPlugin()->OnPluginUnload();
        }

        m_Interfaces.clear();
        m_Plugins.clear();
    }

    TVersion PluginHostImpl::GetHostVersion()
    {
        constexpr TVersion hostVersion{ "1.0.0.0" };
        return hostVersion;
    }

    IPlugin* PluginHostImpl::LoadPlugin(const UId& iid)
    {
        if (auto ctx = FindContext(iid))
        {
            return ctx->GetPlugin();
        }

        auto iidStr = UIdUtils::ToString(iid);
        auto iter   = m_PluginConfig.find(iidStr);
        if (iter == m_PluginConfig.not_found())
        {
            Log::Engine().Warning("Failed to load plugin: '{}' (error: 'plugin not found')", iidStr);
            return nullptr;
        }

        auto pluginPath = iter->second.get_value_optional<String>();
        if (!pluginPath)
        {
            Log::Engine().Warning("Failed to load plugin: '{}' (error: 'plugin path not found')", iidStr);
            return nullptr;
        }

        try
        {
            return m_Plugins.emplace(iid, std::make_unique < PluginContext>(*pluginPath, iid)).first->second->GetPlugin();
        }
        catch (const std::exception& e)
        {
            Log::Engine().Warning("Failed to load plugin: '{}' (error: '{}')", iidStr, e.what());
            return nullptr;
        }
    }

    bool PluginHostImpl::UnloadPlugin(const UId& iid)
    {
        auto ctx = FindContext(iid);
        if (ctx)
        {
            UnloadPlugin_Internal(*ctx);
            m_Plugins.erase(iid);
            return true;
        }
        return false;
    }

    //

    void PluginHostImpl::UnloadPlugin_Internal(PluginContext& context)
    {
        auto plugin = context.GetPlugin();

        std::erase_if(
            m_Plugins,
            [this, plugin](auto& pair)
            {
                if (!pair.second->HasDependencies())
                {
                    return false;
                }

                bool erase = false;
                if (pair.second->GetDependencies().contains(plugin))
                {
                    this->UnloadPlugin_Internal(*pair.second);
                    erase = true;
                }
                return erase;
            });

        std::erase_if(m_Interfaces,
                      [plugin](const auto& pair)
                      {
                          if (pair.second.Owner == plugin)
                          {
                              plugin->OnDropInterface(pair.second.Object);
                              return true;
                          }
                          return false;
                      });

        plugin->OnPluginUnload();
    }
} // namespace Ame