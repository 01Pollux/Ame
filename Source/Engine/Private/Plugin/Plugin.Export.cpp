#include <Plugin/Plugin.hpp>
#include <Plugin/Plugin.Export.h>

class PluginImpl : public Ame::IPlugin
{
public:
    PluginImpl(const Ame_PluginInfo_t* desc) :
        Ame::IPlugin({ desc->Name,
                       desc->Author,
                       desc->Description,
                       desc->Date,
                       { desc->Version.Major, desc->Version.Minor, desc->Version.Build, desc->Version.Revision } })
    {
    }

    bool OnPluginPreLoad(Ame::IModuleRegistry* registry) override
    {
        if (OnPreloadFn)
        {
            return OnPreloadFn(std::bit_cast<Ame_IPlugin_t*>(this), std::bit_cast<Ame_IModuleRegistry_t*>(registry));
        }
        return true;
    }

    void OnPluginLoad(Ame::IModuleRegistry* registry) override
    {
        if (OnLoadFn)
        {
            OnLoadFn(std::bit_cast<Ame_IPlugin_t*>(this), std::bit_cast<Ame_IModuleRegistry_t*>(registry));
        }
    }

    void OnInterfaceDrop(Ame::IObject* iface) override
    {
        if (OnInterfaceDropFn)
        {
            OnInterfaceDropFn(std::bit_cast<Ame_IPlugin_t*>(this), std::bit_cast<Ame_IObject_t*>(iface));
        }
    }

    void OnPluginUnload() override
    {
        if (OnUnloadFn)
        {
            OnUnloadFn(std::bit_cast<Ame_IPlugin_t*>(this));
        }
    }

    void OnPluginPauseChange(bool) override
    {
        if (OnPauseChangedFn)
        {
            OnPauseChangedFn(std::bit_cast<Ame_IPlugin_t*>(this), false);
        }
    }

public:
    bool(AME_CDECL* OnPreloadFn)(Ame_IPlugin_t*, Ame_IModuleRegistry_t*) = nullptr;
    void(AME_CDECL* OnLoadFn)(Ame_IPlugin_t*, Ame_IModuleRegistry_t*)    = nullptr;
    void(AME_CDECL* OnInterfaceDropFn)(Ame_IPlugin_t*, Ame_IObject_t*)   = nullptr;
    void(AME_CDECL* OnUnloadFn)(Ame_IPlugin_t*)                          = nullptr;
    void(AME_CDECL* OnPauseChangedFn)(Ame_IPlugin_t*, bool)              = nullptr;
};

//

Ame_IPlugin_t* Ame_Plugin_Create(const Ame_PluginInfo_t* desc)
{
    return std::bit_cast<Ame_IPlugin_t*>(new PluginImpl(desc));
}

void Ame_Plugin_Release(Ame_IPlugin_t* plugin)
{
    delete std::bit_cast<Ame::IPlugin*>(plugin);
}

void Ame_Plugin_SetOnPreLoad(Ame_IPlugin_t* plugin, bool(AME_CDECL* callback)(Ame_IPlugin_t*, Ame_IModuleRegistry_t*))
{
    auto impl         = std::bit_cast<PluginImpl*>(plugin);
    impl->OnPreloadFn = callback;
}

void Ame_Plugin_SetOnLoad(Ame_IPlugin_t* plugin, void(AME_CDECL* callback)(Ame_IPlugin_t*, Ame_IModuleRegistry_t*))
{
    auto impl      = std::bit_cast<PluginImpl*>(plugin);
    impl->OnLoadFn = callback;
}

void Ame_Plugin_SetOnInterfaceDrop(Ame_IPlugin_t* plugin, void(AME_CDECL* callback)(Ame_IPlugin_t*, Ame_IObject_t*))
{
    auto impl               = std::bit_cast<PluginImpl*>(plugin);
    impl->OnInterfaceDropFn = callback;
}

void Ame_Plugin_SetOnUnload(Ame_IPlugin_t* plugin, void(AME_CDECL* callback)(Ame_IPlugin_t*))
{
    auto impl        = std::bit_cast<PluginImpl*>(plugin);
    impl->OnUnloadFn = callback;
}

void Ame_Plugin_SetOnPauseChanged(Ame_IPlugin_t* plugin, void(AME_CDECL* callback)(Ame_IPlugin_t*, bool))
{
    auto impl              = std::bit_cast<PluginImpl*>(plugin);
    impl->OnPauseChangedFn = callback;
}

Ame_PluginInfo_t Ame_Plugin_GetPluginInfo(Ame_IPlugin_t* plugin)
{
    auto  impl = std::bit_cast<PluginImpl*>(plugin);
    auto& info = impl->GetPluginInfo();
    return { info.Name,
             info.Author,
             info.Description,
             info.Date,
             { info.Version.Major(), info.Version.Minor(), info.Version.Build(), info.Version.Revision() } };
}

Ame_StringView_t Ame_Plugin_GetPluginName(Ame_IPlugin_t* plugin)
{
    auto  impl = std::bit_cast<PluginImpl*>(plugin);
    auto& name = impl->GetPluginName();
    return { name.c_str(), impl->GetPluginName().size() };
}

Ame_StringView_t Ame_Plugin_GetPluginPath(Ame_IPlugin_t* plugin)
{
    auto impl = std::bit_cast<PluginImpl*>(plugin);
    return { impl->GetPluginPath().c_str(), impl->GetPluginPath().size() };
}

bool Ame_Plugin_IsPaused(Ame_IPlugin_t* plugin)
{
    auto impl = std::bit_cast<PluginImpl*>(plugin);
    return impl->IsPluginPaused();
}

void Ame_Plugin_SetPauseState(Ame_IPlugin_t* plugin, bool paused)
{
    auto impl = std::bit_cast<PluginImpl*>(plugin);
    impl->SetPluginState(paused);
}
