#pragma once

#include <Core/Interface.Export.h>
#include <Core/Version.Export.h>

typedef struct Ame_IModuleRegistry_t Ame_IModuleRegistry_t;
typedef struct Ame_IPlugin_t         Ame_IPlugin_t;

typedef struct Ame_PluginInfo_t
{
    const char*   Name;
    const char*   Author;
    const char*   Description;
    const char*   Date;
    Ame_Version_t Version;
} Ame_PluginInfo_t;

// clang-format off
extern "C"
{
    AME_ENGINE_C(Ame_IPlugin_t*, Ame_Plugin_Create(const Ame_PluginInfo_t* desc));
    AME_ENGINE_C(void, Ame_Plugin_Release(Ame_IPlugin_t* plugin));

    AME_ENGINE_C(void, Ame_Plugin_SetOnPreLoad(Ame_IPlugin_t* plugin, bool(AME_CDECL* callback)(Ame_IPlugin_t*, Ame_IModuleRegistry_t*)));
    AME_ENGINE_C(void, Ame_Plugin_SetOnLoad(Ame_IPlugin_t* plugin, void(AME_CDECL* callback)(Ame_IPlugin_t*, Ame_IModuleRegistry_t*)));
    AME_ENGINE_C(void, Ame_Plugin_SetOnInterfaceDrop(Ame_IPlugin_t* plugin, void(AME_CDECL* callback)(Ame_IPlugin_t*, Ame_IBaseObject_t*)));
    AME_ENGINE_C(void, Ame_Plugin_SetOnUnload(Ame_IPlugin_t* plugin, void(AME_CDECL* callback)(Ame_IPlugin_t*)));
    AME_ENGINE_C(void, Ame_Plugin_SetOnPauseChanged(Ame_IPlugin_t* plugin, void(AME_CDECL* callback)(Ame_IPlugin_t*, bool)));

    AME_ENGINE_C(Ame_PluginInfo_t, Ame_Plugin_GetPluginInfo(Ame_IPlugin_t* plugin));
    AME_ENGINE_C(Ame_StringView_t, Ame_Plugin_GetPluginName(Ame_IPlugin_t* plugin));
    AME_ENGINE_C(Ame_StringView_t, Ame_Plugin_GetPluginPath(Ame_IPlugin_t* plugin));
    AME_ENGINE_C(bool, Ame_Plugin_IsPaused(Ame_IPlugin_t* plugin));
    AME_ENGINE_C(void, Ame_Plugin_SetPauseState(Ame_IPlugin_t* plugin, bool paused));
}
// clang-format on