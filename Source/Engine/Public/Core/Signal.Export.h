#pragma once

#include <Core/CApi.h>

typedef struct Ame_Signal_t           Ame_Signal_t;
typedef struct Ame_SignalConnection_t Ame_SignalConnection_t;
typedef void(AME_CDECL* Ame_SignalSlot_t)(void* /*userData*/);

// clang-format off
extern "C"
{
    AME_ENGINE_C(Ame_Signal_t*, Ame_Signal_Create());
    AME_ENGINE_C(void, Ame_Signal_Release(Ame_Signal_t* signalHandle));

    AME_ENGINE_C(Ame_SignalConnection_t*, Ame_Signal_Connect(Ame_Signal_t* signalHandle, Ame_SignalSlot_t slot, void* userData));
    AME_ENGINE_C(void, Ame_Signal_Disconnect(Ame_SignalConnection_t* connectionHandle));

    AME_ENGINE_C(void, Ame_Signal_Invoke(Ame_Signal_t* signalHandle));
}
// clang-format on
