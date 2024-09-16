#include <Core/Interface.hpp>
#include <Core/Interface.Export.h>

void Ame_IBaseObject_AddRef(Ame_IObject_t* objectHandle)
{
    auto object = std::bit_cast<Ame::IObject*>(objectHandle);
    object->AddRef();
}

void Ame_IBaseObject_Release(Ame_IObject_t* objectHandle)
{
    auto object = std::bit_cast<Ame::IObject*>(objectHandle);
    object->Release();
}

auto Ame_IBaseObject_QueryInterface(Ame_IObject_t* objectHandle, const Ame_UID_t* iidHandle) -> Ame_IObject_t*
{
    auto  object = std::bit_cast<Ame::IObject*>(objectHandle);
    auto& iid    = *std::bit_cast<const Ame::UId*>(iidHandle);

    Ame::IObject* obj = nullptr;
    object->QueryInterface(iid, &obj);

    return std::bit_cast<Ame_IObject_t*>(obj);
}