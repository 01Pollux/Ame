#include <Engine/Engine.hpp>
#include <Engine/Engine.Export.h>

Ame_Engine_t* Ame_Engine_Create()
{
    return std::bit_cast<Ame_Engine_t*>(new Ame::AmeEngine);
}

void Ame_Engine_Release(Ame_Engine_t* engine)
{
    delete std::bit_cast<Ame::AmeEngine*>(engine);
}

void Ame_Engine_RefreshSubmoduleCache(Ame_Engine_t* engine)
{
    std::bit_cast<Ame::AmeEngine*>(engine)->RefreshSubmoduleCache();
}

void Ame_Engine_Tick(Ame_Engine_t* engine)
{
    std::bit_cast<Ame::AmeEngine*>(engine)->Tick();
}

bool Ame_Engine_IsRunning(Ame_Engine_t* engine)
{
    return std::bit_cast<Ame::AmeEngine*>(engine)->IsRunning();
}

void Ame_Engine_Exit(Ame_Engine_t* engine, int exitCode)
{
    std::bit_cast<Ame::AmeEngine*>(engine)->Exit(exitCode);
}

Ame_IModuleRegistry_t* Ame_Engine_GetModuleRegistry(Ame_Engine_t* engine)
{
    return std::bit_cast<Ame_IModuleRegistry_t*>(std::bit_cast<Ame::AmeEngine*>(engine)->GetRegistry());
}