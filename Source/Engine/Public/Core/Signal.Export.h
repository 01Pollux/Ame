#pragma once

#include <Core/CApi.h>

typedef struct Ame_Signal_t           Ame_Signal_t;
typedef struct Ame_SignalConnection_t Ame_SignalConnection_t;
typedef void (*Ame_SignalSlot_t)(void* /*args*/, void* /*userData*/);

// clang-format off
extern "C"
{
    AME_EXPORT_C(Ame_Signal_t*, Ame_Signal_Create());
    AME_EXPORT_C(void, Ame_Signal_Release(Ame_Signal_t* signalHandle));

    AME_EXPORT_C(Ame_SignalConnection_t*, Ame_Signal_Connect(Ame_Signal_t* signalHandle, Ame_SignalSlot_t slot, void* userData));
    AME_EXPORT_C(void, Ame_Signal_Disconnect(Ame_SignalConnection_t* connectionHandle));

    AME_EXPORT_C(void, Ame_Signal_Invoke(Ame_Signal_t* signalHandle, void* args));
}
// clang-format on
