#pragma once

#include <Core/Interface.Export.h>
#include <Core/Version.Export.h>

typedef struct Ame_IModuleRegistry_t Ame_IModuleRegistry_t;
typedef struct Ame_IPlugin_t         Ame_IPlugin_t;

// clang-format off
extern "C"
{
    AME_ENGINE_C(Ame_IModuleRegistry_t*, Ame_ModuleRegistry_Create());
    AME_ENGINE_C(void, Ame_ModuleRegistry_Release(Ame_IModuleRegistry_t* registry));
    AME_ENGINE_C(struct Ame_Version_t, Ame_ModuleRegistry_GetHostVersion(Ame_IModuleRegistry_t* registry));

    AME_ENGINE_C(void, Ame_ModuleRegistry_ExposeInterface(Ame_IModuleRegistry_t* registry, Ame_IPlugin_t* owner, const Ame_UID_t* iid, Ame_IObject_t* iface));
    AME_ENGINE_C(void, Ame_ModuleRegistry_DropInterface(Ame_IModuleRegistry_t* registry, const Ame_UID_t* iid));
    AME_ENGINE_C(Ame_IObject_t*, Ame_ModuleRegistry_RequestInterface(Ame_IModuleRegistry_t* registry, Ame_IPlugin_t* caller, const Ame_UID_t* iid));

    AME_ENGINE_C(Ame_IPlugin_t*, Ame_ModuleRegistry_FindPlugin(Ame_IModuleRegistry_t* registry, const Ame_StringView_t name));
    AME_ENGINE_C(Ame_IPlugin_t*, Ame_ModuleRegistry_BindPlugin(Ame_IModuleRegistry_t* registry, Ame_IPlugin_t* caller, const Ame_StringView_t name, bool isRequired));
    AME_ENGINE_C(Ame_IPlugin_t*, Ame_ModuleRegistry_LoadPlugin(Ame_IModuleRegistry_t* registry, const Ame_StringView_t name));
    AME_ENGINE_C(bool, Ame_ModuleRegistry_UnloadPlugin(Ame_IModuleRegistry_t* registry, Ame_IPlugin_t* plugin));
    AME_ENGINE_C(bool, Ame_ModuleRegistry_UnloadPluginByName(Ame_IModuleRegistry_t* registry, Ame_StringView_t name));
}
// clang-format on