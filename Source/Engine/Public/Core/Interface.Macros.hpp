#pragma once

#define AmeCreateRaw(Ty, ...)         (Ame::ObjectAllocator<Ty>()(__VA_ARGS__))
#define AmeCreateRawAs(Ty, AsTy, ...) (static_cast<AsTy*>(Ame::ObjectAllocator<Ty>()(__VA_ARGS__)))
#define AmeCreateRawObject(Ty, ...)   (static_cast<Ame::IObject*>(Ame::ObjectAllocator<Ty>()(__VA_ARGS__)))
#define AmeCreate(Ty, ...)            (Ptr(AmeCreateRaw(Ty, __VA_ARGS__)))
#define AmeCreateAs(Ty, AsTy, ...)    (Ame::Ptr<AsTy>{ AmeCreateRawAs(Ty, AsTy, __VA_ARGS__) })
#define AmeCreateObject(Ty, ...)      (Ame::Ptr<Ame::IObject>{ AmeCreateRawObject(Ty, __VA_ARGS__) })

//

#define IMPLEMENT_INTERFACE_CTOR(ClassName, ...)                                                                       \
    template<typename, typename> friend class Dg::MakeNewRCObj;                                                        \
    explicit ClassName(IReferenceCounters* counters, __VA_ARGS__)

//

#define IMPLEMENT_QUERY_INTERFACE_DECL()                                                                               \
    void DILIGENT_CALL_TYPE QueryInterface(const INTERFACE_ID& iid, IObject** outObject) override

#define IMPLEMENT_QUERY_INTERFACE_HEAD(ClassName)                                                                      \
    void ClassName::QueryInterface(const INTERFACE_ID& iid, IObject** outObject)

//

#define IMPLEMENT_QUERY_INTERFACE_SUBOJECTS2_BODY(InterfaceID1, InterfaceID2, ParentClassName, ...)                    \
    {                                                                                                                  \
        if (iid == InterfaceID1 || iid == InterfaceID2)                                                                \
        {                                                                                                              \
            *outObject = this;                                                                                         \
            (*outObject)->AddRef();                                                                                    \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            IObject* const subObjects[]{ __VA_ARGS__ };                                                                \
                                                                                                                       \
            for (auto subObject : subObjects)                                                                          \
            {                                                                                                          \
                if (subObject)                                                                                         \
                {                                                                                                      \
                    subObject->QueryInterface(iid, outObject);                                                         \
                    if (*outObject)                                                                                    \
                    {                                                                                                  \
                        return;                                                                                        \
                    }                                                                                                  \
                }                                                                                                      \
            }                                                                                                          \
                                                                                                                       \
            ParentClassName::QueryInterface(iid, outObject);                                                           \
        }                                                                                                              \
    }

#define IMPLEMENT_QUERY_INTERFACE_SUBOJECTS_BODY(InterfaceID, ParentClassName, ...)                                    \
    {                                                                                                                  \
        if (iid == InterfaceID)                                                                                        \
        {                                                                                                              \
            *outObject = this;                                                                                         \
            (*outObject)->AddRef();                                                                                    \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            ParentClassName::QueryInterface(iid, outObject);                                                           \
            if (*outObject)                                                                                            \
            {                                                                                                          \
                return;                                                                                                \
            }                                                                                                          \
                                                                                                                       \
            IObject* const subObjects[]{ __VA_ARGS__ };                                                                \
            for (auto subObject : subObjects)                                                                          \
            {                                                                                                          \
                if (subObject)                                                                                         \
                {                                                                                                      \
                    subObject->QueryInterface(iid, outObject);                                                         \
                    if (*outObject)                                                                                    \
                    {                                                                                                  \
                        return;                                                                                        \
                    }                                                                                                  \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    }

//

#define IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS(InterfaceID, ParentClassName, ...)                                \
    IMPLEMENT_QUERY_INTERFACE_DECL()                                                                                   \
    IMPLEMENT_QUERY_INTERFACE_SUBOJECTS_BODY(InterfaceID, ParentClassName, __VA_ARGS__)

#define IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(InterfaceID1, InterfaceID2, ParentClassName, ...)                \
    IMPLEMENT_QUERY_INTERFACE_DECL()                                                                                   \
    IMPLEMENT_QUERY_INTERFACE_SUBOJECTS2_BODY(InterfaceID1, InterfaceID2, ParentClassName, __VA_ARGS__)

//

#define IMPLEMENT_QUERY_INTERFACE_SUBOJECTS(InterfaceID, ParentClassName, ...)                                         \
    IMPLEMENT_QUERY_INTERFACE_HEAD()                                                                                   \
    IMPLEMENT_QUERY_INTERFACE_SUBOJECTS_BODY(InterfaceID, ParentClassName, __VA_ARGS__)

#define IMPLEMENT_QUERY_INTERFACE_SUBOJECTS2(InterfaceID1, InterfaceID2, ParentClassName, ...)                         \
    IMPLEMENT_QUERY_INTERFACE_HEAD()                                                                                   \
    IMPLEMENT_QUERY_INTERFACE_SUBOJECTS2_BODY(InterfaceID1, InterfaceID2, ParentClassName, __VA_ARGS__)
