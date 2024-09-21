#include <Core/Interface.hpp>
#include <Core/Interface.Export.h>

class AmeBaseObjectImpl : public Ame::IObjectWithCallback
{
private:
    IMPLEMENT_INTERFACE_CTOR(AmeBaseObjectImpl) : IObjectWithCallback(counters)
    {
    }
};

void Ame_IBaseObject_AddRef(Ame_IBaseObject_t* objectHandle)
{
    auto object = std::bit_cast<Ame::IObject*>(objectHandle);
    object->AddRef();
}

void Ame_IBaseObject_Release(Ame_IBaseObject_t* objectHandle)
{
    auto object = std::bit_cast<Ame::IObject*>(objectHandle);
    object->Release();
}

Ame_IBaseObject_t* Ame_IBaseObject_QueryInterface(Ame_IBaseObject_t* objectHandle, const Ame_UID_t* iidHandle)
{
    auto  object = std::bit_cast<Ame::IObject*>(objectHandle);
    auto& iid    = *std::bit_cast<const Ame::UId*>(iidHandle);

    Ame::IObject* obj = nullptr;
    object->QueryInterface(iid, &obj);

    return std::bit_cast<Ame_IBaseObject_t*>(obj);
}

Ame_IBaseObject_t* Ame_BaseObject_Create()
{
    return std::bit_cast<Ame_IBaseObject_t*>(AmeCreateRaw(AmeBaseObjectImpl));
}

Ame_BaseObject_QueryInterface_t Ame_IBaseObject_SetQueryInterface(Ame_IBaseObject_t*              objectHandle,
                                                                  Ame_BaseObject_QueryInterface_t callback)
{
    auto object = std::bit_cast<Ame::IObject*>(objectHandle);
    auto impl   = dynamic_cast<Ame::IObjectWithCallback*>(object);
    if (impl)
    {
        return std::bit_cast<Ame_BaseObject_QueryInterface_t>(std::exchange(
            impl->QueryInterfaceCallback, std::bit_cast<Ame::IObjectWithCallback::QueryCallback>(callback)));
    }
    return nullptr;
}