#include <Plugin/PluginHostImpl.hpp>

#include <Log/Wrapper.hpp>

namespace Ame
{
    PluginHostImpl::~PluginHostImpl()
    {
        ReleaseAllPlugins();
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

    IPlugin* PluginHostImpl::FindPlugin(const String& name)
    {
        auto ctx = FindContext(name);
        return ctx ? ctx->GetPlugin() : nullptr;
    }

    IPlugin* PluginHostImpl::BindPlugin(IPlugin* caller, const String& name, bool isRequired)
    {
        IPlugin* plugin = nullptr;

        auto callerCtx = FindContext(caller);
        if (auto ctx = FindContext(name))
        {
            plugin = ctx->GetPlugin();
        }
        else if (isRequired)
        {
            plugin = LoadPlugin(name);
        }

        if (plugin)
        {
            callerCtx->AddDependencies(plugin);
        }
        return plugin;
    }

    void PluginHostImpl::Shutdown()
    {
        ReleaseAllPlugins();
    }

    TVersion PluginHostImpl::GetHostVersion()
    {
        constexpr TVersion hostVersion{ "1.0.0.0" };
        return hostVersion;
    }

    IPlugin* PluginHostImpl::LoadPlugin(const String& name)
    {
        if (auto ctx = FindContext(name))
        {
            return ctx->GetPlugin();
        }

        try
        {
            auto ctx    = std::make_unique<PluginContext>(name);
            auto plugin = ctx->GetPlugin();

            plugin->OnPluginPreLoad(this);
            if (plugin->OnPluginLoad(this))
            {
                m_Plugins.emplace(name, std::move(ctx));
                return plugin;
            }
        }
        catch (const std::exception& e)
        {
            Log::Engine().Warning("Failed to load plugin: '{}' (error: '{}')", name, e.what());
        }
        return nullptr;
    }

    bool PluginHostImpl::UnloadPlugin(const String& name)
    {
        auto ctx = FindContext(name);
        if (ctx)
        {
            UnloadPlugin_Internal(*ctx);
            m_Plugins.erase(name);
            return true;
        }
        return false;
    }

    //

    void PluginHostImpl::ReleaseAllPlugins()
    {
        for (auto& iface : m_Interfaces | std::views::values)
        {
            iface.Owner->OnDropInterface(iface.Object);
        }

        m_Plugins.clear();
        m_Interfaces.clear();
    }

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
    }
} // namespace Ame