#pragma once

typedef struct Ame_Engine_t          AmeEngine_t;
typedef struct Ame_IModuleRegistry_t Ame_IModuleRegistry_t;

// clang-format off
extern "C"
{
    AME_ENGINE_C(Ame_Engine_t*, Ame_Engine_Create());
    AME_ENGINE_C(void, Ame_Engine_Release(Ame_Engine_t* engine));

    AME_ENGINE_C(void, Ame_Engine_RefreshSubmoduleCache(Ame_Engine_t* engine));
    AME_ENGINE_C(void, Ame_Engine_Tick(Ame_Engine_t* engine));
    AME_ENGINE_C(bool, Ame_Engine_IsRunning(Ame_Engine_t* engine));
    AME_ENGINE_C(void, Ame_Engine_Exit(Ame_Engine_t* engine, int exitCode));
    AME_ENGINE_C(Ame_IModuleRegistry_t*, Ame_Engine_GetModuleRegistry(Ame_Engine_t* engine));
}
// clang-format on
