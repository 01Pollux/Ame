#pragma once

#include <Core/CApi.h>
#include <Core/Signal.Export.h>

typedef struct Ame_FrameEvent_t Ame_FrameEvent_t;
typedef void(AME_CDECL* Ame_FrameEvent_SignalSlot_t)(void* /*userData*/);

// clang-format off
extern "C"
{
    // Must be released with Ame_IBaseObject_Release
    AME_ENGINE_C(Ame_FrameEvent_t*, Ame_FrameEvent_Create());

    AME_ENGINE_C(Ame_SignalConnection_t*, Ame_FrameEvent_OnFrameStart_Connect(Ame_FrameEvent_t* frameEventHandle, Ame_FrameEvent_SignalSlot_t slot, void* userData));
    AME_ENGINE_C(Ame_SignalConnection_t*, Ame_FrameEvent_OnFrameUpdate_Connect(Ame_FrameEvent_t* frameEventHandle, Ame_FrameEvent_SignalSlot_t slot, void* userData));
    AME_ENGINE_C(Ame_SignalConnection_t*, Ame_FrameEvent_OnFrameEnd_Connect(Ame_FrameEvent_t* frameEventHandle, Ame_FrameEvent_SignalSlot_t slot, void* userData));
}
// clang-format on