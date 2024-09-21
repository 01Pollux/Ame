#include <bit>
#include <Core/Signal.hpp>
#include <Core/Signal.Export.h>

using SignalImpl = Ame::Signals::Signal<void()>;

Ame_Signal_t* Ame_Signal_Create()
{
    return std::bit_cast<Ame_Signal_t*>(new SignalImpl);
}

void Ame_Signal_Release(Ame_Signal_t* signalHandle)
{
    delete std::bit_cast<SignalImpl*>(signalHandle);
}

Ame_SignalConnection_t* Ame_Signal_Connect(Ame_Signal_t* signalHandle, Ame_SignalSlot_t slot, void* userData)
{
    auto signal = std::bit_cast<SignalImpl*>(signalHandle);
    return Ame::Signals::WrapSignalCallback(*signal, slot, userData);
}

void Ame_Signal_Disconnect(Ame_SignalConnection_t* connectionHandle)
{
    auto connection = std::bit_cast<Ame::Signals::Connection*>(connectionHandle);
    connection->disconnect();
    delete connection;
}

void Ame_Signal_Invoke(void* signalHandle)
{
    auto signal = std::bit_cast<SignalImpl*>(signalHandle);
    signal->Invoke();
}
