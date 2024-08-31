#include <Plugin/ModuleRegistryImpl.hpp>

#include <Log/Logger.hpp>

namespace Ame
{
    ModuleRegistryImpl::~ModuleRegistryImpl()
    {
        ReleaseAllPlugins();
    }

    //

    bool ModuleRegistryImpl::ExposeInterface(IPlugin* owner, const UId& iid, IObject* object)
    {
        auto iter = m_Interfaces.find(iid);
        if (iter != m_Interfaces.end())
        {
            return false;
        }

        m_Interfaces.emplace(iid, InterfaceContext{ owner, object });
        return true;
    }

    bool ModuleRegistryImpl::RequestInterface(IPlugin* caller, const UId& iid, IObject** iface)
    {
        auto iter = m_Interfaces.find(iid);
        if (iter == m_Interfaces.end())
        {
            return false;
        }
        if (caller)
        {
            iter->second.AddDependencies(caller);
        }

        *iface = iter->second.GetObject();
        (*iface)->AddRef();
        return true;
    }

    bool ModuleRegistryImpl::DropInterface(const UId& iid)
    {
        auto iter = m_Interfaces.find(iid);
        if (iter == m_Interfaces.end())
        {
            return false;
        }
        if (iter->second.HasDependencies())
        {
            return false;
        }
        m_Interfaces.erase(iter);
        return true;
    }

    //

    IPlugin* ModuleRegistryImpl::FindPlugin(const String& name)
    {
        auto ctx = FindContext(name);
        return ctx ? ctx->GetPlugin() : nullptr;
    }

    IPlugin* ModuleRegistryImpl::BindPlugin(IPlugin* caller, const String& name, bool isRequired)
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

    //

    void ModuleRegistryImpl::Shutdown()
    {
        ReleaseAllPlugins();
    }

    TVersion ModuleRegistryImpl::GetHostVersion()
    {
        constexpr TVersion hostVersion{ "1.1.0.0" };
        return hostVersion;
    }

    //

    IPlugin* ModuleRegistryImpl::LoadPlugin(const String& name)
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
            AME_LOG_WARNING(std::format("Failed to load plugin: '{}' (error: '{}')", name, e.what()));
        }
        return nullptr;
    }

    bool ModuleRegistryImpl::UnloadPlugin(const String& name)
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

    void ModuleRegistryImpl::ReleaseAllPlugins()
    {
        m_Interfaces.clear();
        m_Plugins.clear();
    }

    void ModuleRegistryImpl::UnloadPlugin_Internal(PluginContext& context)
    {
        auto plugin = context.GetPlugin();

        std::erase_if(m_Plugins,
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

        std::erase_if(m_Interfaces, [plugin](const auto& pair) { return pair.second.GetPlugin() == plugin; });
    }
} // namespace Ame