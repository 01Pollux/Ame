#include <Interfaces/Core/FrameEvent.hpp>
#include <Interfaces/Core/FrameEvent.Export.h>

Ame_FrameEvent_t* Ame_FrameEvent_Create()
{
    return std::bit_cast<Ame_FrameEvent_t*>(AmeCreateRaw(Ame::Interfaces::FrameEvent));
}

Ame_SignalConnection_t* Ame_FrameEvent_OnFrameStart_Connect(Ame_FrameEvent_t* frameEventHandle, Ame_SignalSlot_t slot,
                                                            void* userData)
{
    auto frameEvent = std::bit_cast<Ame::Interfaces::FrameEvent*>(frameEventHandle);
    return Ame::Signals::WrapSignalCallback(frameEvent->OnFrameStart, slot, userData);
}

Ame_SignalConnection_t* Ame_FrameEvent_OnFrameUpdate_Connect(Ame_FrameEvent_t* frameEventHandle, Ame_SignalSlot_t slot,
                                                             void* userData)
{
    auto frameEvent = std::bit_cast<Ame::Interfaces::FrameEvent*>(frameEventHandle);
    return Ame::Signals::WrapSignalCallback(frameEvent->OnFrameUpdate, slot, userData);
}

Ame_SignalConnection_t* Ame_FrameEvent_OnFrameEnd_Connect(Ame_FrameEvent_t*           frameEventHandle,
                                                          Ame_FrameEvent_SignalSlot_t slot, void* userData)
{
    auto frameEvent = std::bit_cast<Ame::Interfaces::FrameEvent*>(frameEventHandle);
    return Ame::Signals::WrapSignalCallback(frameEvent->OnFrameEnd, slot, userData);
}
