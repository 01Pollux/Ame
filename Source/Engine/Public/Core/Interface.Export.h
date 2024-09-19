#pragma once

typedef struct Ame_IObject_t Ame_IObject_t;
typedef struct Ame_UID_t     Ame_UID_t;

// clang-format off
extern "C"
{
    AME_ENGINE_C(void, Ame_IBaseObject_AddRef(Ame_IObject_t* objectHandle));
    AME_ENGINE_C(void, Ame_IBaseObject_Release(Ame_IObject_t* objectHandle));
    AME_ENGINE_C(Ame_IObject_t*, Ame_IBaseObject_QueryInterface(Ame_IObject_t* objectHandle, const Ame_UID_t* iidHandle));
}
// clang-format on