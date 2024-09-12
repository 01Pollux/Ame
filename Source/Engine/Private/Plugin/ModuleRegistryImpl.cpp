#include <Plugin/ModuleRegistryImpl.hpp>
#include <Core/Coroutine.hpp>

#include <Log/Logger.hpp>

namespace Ame
{
    static UniquePtr<Co::runtime> s_Coroutine;

    Co::runtime& Coroutine::Get() noexcept
    {
        return *s_Coroutine;
    }

    //

    ModuleRegistryImpl::ModuleRegistryImpl()
    {
        s_Coroutine = std::make_unique<Co::runtime>();
    }

    ModuleRegistryImpl::~ModuleRegistryImpl()
    {
        ReleaseAllPlugins();
        s_Coroutine = nullptr;
    }

    //

    bool ModuleRegistryImpl::ExposeInterface(IPlugin* owner, const UId& iid, IObject* object)
    {
        auto iter = m_Interfaces.find(iid);
        if (iter != m_Interfaces.end())
        {
            AME_LOG_WARNING(std::format("Interface '{}' not found", UIdUtils::ToString(iid)));
            return false;
        }

        AME_LOG_ASSERT(object != nullptr, "Invalid interface pointer");
        m_Interfaces.emplace(iid, InterfaceContext{ owner, object });
        return true;
    }

    bool ModuleRegistryImpl::RequestInterface(IPlugin* caller, const UId& iid, IObject** object)
    {
        auto iter = m_Interfaces.find(iid);
        if (iter == m_Interfaces.end())
        {
            AME_LOG_WARNING(std::format("Interface '{}' not found", UIdUtils::ToString(iid)));
            return false;
        }

        if (caller)
        {
            AME_LOG_ASSERT(FindContext(caller) != nullptr, "Invalid caller plugin");
            iter->second.AddDependencies(caller);
        }

        AME_LOG_ASSERT(object != nullptr, "Invalid interface pointer");
#ifdef AME_DEBUG
        if (*object != nullptr)
        {
            AME_LOG_WARNING("Interface will be overwritten");
        }
#endif

        *object = iter->second.GetObject();
        (*object)->AddRef();
        return true;
    }

    bool ModuleRegistryImpl::DropInterface(const UId& iid)
    {
        auto iter = m_Interfaces.find(iid);
        if (iter == m_Interfaces.end())
        {
            AME_LOG_WARNING(std::format("Interface '{}' not found", UIdUtils::ToString(iid)));
            return false;
        }
        iter->second.DropDependencies();
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
        AME_LOG_ASSERT(caller != nullptr, "Invalid caller plugin");
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

    TVersion ModuleRegistryImpl::GetHostVersion()
    {
        constexpr TVersion hostVersion{ "1.2.0.0" };
        return hostVersion;
    }

    //

    IPlugin* ModuleRegistryImpl::LoadPlugin(const String& name)
    {
        if (auto ctx = FindContext(name))
        {
            return ctx->GetPlugin();
        }

        PluginContext* ctxPtr = nullptr;
        try
        {
            auto ctx = std::make_unique<PluginContext>(name);
            ctxPtr   = ctx.get();

            auto plugin = ctx->GetPlugin();
            m_Plugins.emplace(name, std::move(ctx));

            if (plugin->OnPluginPreLoad(this))
            {
                plugin->OnPluginLoad(this);
                return plugin;
            }
        }
        catch (const std::exception& e)
        {
            AME_LOG_WARNING(std::format("Failed to load plugin: '{}' (error: '{}')", name, e.what()));
        }

        if (ctxPtr)
        {
            ctxPtr->Invalidate();
            m_Plugins.erase(name);
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
        for (auto& iface : m_Interfaces | std::views::values)
        {
            iface.DropInterface();
        }
        m_Plugins.clear();
        m_Interfaces.clear();
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

        std::erase_if(m_Interfaces,
                      [plugin](auto& pair)
                      {
                          bool erase = false;
                          if (pair.second.GetPlugin() == plugin)
                          {
                              pair.second.DropDependencies();
                              erase = true;
                          }

                          pair.second.RemoveDependencies(plugin);
                          return erase;
                      });
    }
} // namespace Ame