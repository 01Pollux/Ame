#pragma once

typedef struct Ame_IBaseObject_t Ame_IBaseObject_t;
typedef struct Ame_UID_t     Ame_UID_t;
typedef Ame_IBaseObject_t*(AME_CDECL* Ame_BaseObject_QueryInterface_t)(Ame_IBaseObject_t* objectHandle, const Ame_UID_t* iidHandle);

// clang-format off
extern "C"
{
    AME_ENGINE_C(void, Ame_IBaseObject_AddRef(Ame_IBaseObject_t* objectHandle));
    AME_ENGINE_C(void, Ame_IBaseObject_Release(Ame_IBaseObject_t* objectHandle));
    AME_ENGINE_C(Ame_IBaseObject_t*, Ame_IBaseObject_QueryInterface(Ame_IBaseObject_t* objectHandle, const Ame_UID_t* iidHandle));

    AME_ENGINE_C(Ame_IBaseObject_t*, Ame_BaseObject_Create());
    AME_ENGINE_C(Ame_BaseObject_QueryInterface_t, Ame_IBaseObject_SetQueryInterface(Ame_IBaseObject_t* objectHandle, Ame_BaseObject_QueryInterface_t callback));
}
// clang-format on