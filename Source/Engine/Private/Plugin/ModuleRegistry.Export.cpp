#include <Plugin/ModuleRegistryImpl.hpp>
#include <Plugin/ModuleRegistry.Export.h>

Ame_IModuleRegistry_t* Ame_ModuleRegistry_Create()
{
    return std::bit_cast<Ame_IModuleRegistry_t*>(static_cast<Ame::IModuleRegistry*>(new Ame::ModuleRegistryImpl()));
}

void Ame_ModuleRegistry_Release(Ame_IModuleRegistry_t* registry)
{
    delete std::bit_cast<Ame::IModuleRegistry*>(registry);
}

struct Ame_Version_t Ame_ModuleRegistry_GetHostVersion(Ame_IModuleRegistry_t* registry)
{
    auto version = std::bit_cast<Ame::IModuleRegistry*>(registry)->GetHostVersion();
    return { version.Major(), version.Minor(), version.Build(), version.Revision() };
}

void Ame_ModuleRegistry_ExposeInterface(Ame_IModuleRegistry_t* registry, Ame_IPlugin_t* owner, const Ame_UID_t* iid,
                                        Ame_IBaseObject_t* iface)
{
    auto impl      = std::bit_cast<Ame::IModuleRegistry*>(registry);
    auto ownerImpl = std::bit_cast<Ame::IPlugin*>(owner);
    auto iidImpl   = std::bit_cast<Ame::UId*>(iid);
    auto ifaceImpl = std::bit_cast<Ame::IObject*>(iface);
    impl->ExposeInterface(ownerImpl, *iidImpl, ifaceImpl);
}

void Ame_ModuleRegistry_DropInterface(Ame_IModuleRegistry_t* registry, const Ame_UID_t* iid)
{
    auto impl    = std::bit_cast<Ame::IModuleRegistry*>(registry);
    auto iidImpl = std::bit_cast<Ame::UId*>(iid);
    impl->DropInterface(*iidImpl);
}

Ame_IBaseObject_t* Ame_ModuleRegistry_RequestInterface(Ame_IModuleRegistry_t* registry, Ame_IPlugin_t* caller,
                                                   const Ame_UID_t* iid)
{
    auto          impl       = std::bit_cast<Ame::IModuleRegistry*>(registry);
    auto          callerImpl = std::bit_cast<Ame::IPlugin*>(caller);
    auto          iidImpl    = std::bit_cast<Ame::UId*>(iid);
    Ame::IObject* iface      = nullptr;
    return impl->RequestInterface(callerImpl, *iidImpl, &iface) ? std::bit_cast<Ame_IBaseObject_t*>(iface) : nullptr;
}

Ame_IPlugin_t* Ame_ModuleRegistry_FindPlugin(Ame_IModuleRegistry_t* registry, const Ame_StringView_t name)
{
    auto impl = std::bit_cast<Ame::IModuleRegistry*>(registry);
    return std::bit_cast<Ame_IPlugin_t*>(impl->FindPlugin({ name.Data, name.Size }));
}

Ame_IPlugin_t* Ame_ModuleRegistry_BindPlugin(Ame_IModuleRegistry_t* registry, Ame_IPlugin_t* caller,
                                             const Ame_StringView_t name, bool isRequired)
{
    auto impl = std::bit_cast<Ame::IModuleRegistry*>(registry);
    return std::bit_cast<Ame_IPlugin_t*>(
        impl->BindPlugin(std::bit_cast<Ame::IPlugin*>(caller), { name.Data, name.Size }, isRequired));
}

Ame_IPlugin_t* Ame_ModuleRegistry_LoadPlugin(Ame_IModuleRegistry_t* registry, const Ame_StringView_t name)
{
    auto impl = std::bit_cast<Ame::IModuleRegistry*>(registry);
    return std::bit_cast<Ame_IPlugin_t*>(impl->LoadPlugin({ name.Data, name.Size }));
}

bool Ame_ModuleRegistry_UnloadPlugin(Ame_IModuleRegistry_t* registry, Ame_IPlugin_t* plugin)
{
    auto impl = std::bit_cast<Ame::IModuleRegistry*>(registry);
    return impl->UnloadPlugin(std::bit_cast<Ame::IPlugin*>(plugin));
}

bool Ame_ModuleRegistry_UnloadPluginByName(Ame_IModuleRegistry_t* registry, Ame_StringView_t name)
{
    auto impl = std::bit_cast<Ame::IModuleRegistry*>(registry);
    return impl->UnloadPlugin({ name.Data, name.Size });
}
