#pragma once

#include <source_location>

#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#include <DiligentCore/Common/interface/ObjectBase.hpp>

namespace Dg = Diligent;

#define AME_CALL_TYPE DILIGENT_CALL_TYPE
#define AME_METHOD(x) AME_CALL_TYPE x

namespace Ame
{
    using UId          = Dg::INTERFACE_ID;
    using INTERFACE_ID = Dg::INTERFACE_ID;

    using IObject = Dg::IObject;

    using IReferenceCounters = Dg::IReferenceCounters;

    template<typename Ty>
    using RefCountedObject = Dg::RefCountedObject<Ty>;

    template<typename Ty>
    using Ptr = Dg::RefCntAutoPtr<Ty>;

    template<typename Ty>
    using WPtr = Dg::RefCntWeakPtr<Ty>;

    static constexpr UId IID_Unknown = Dg::IID_Unknown;

    //

    struct UIdHasher
    {
        size_t operator()(const UId& id) const;
    };

    //

    class ObjectMemoryAllocator : public Dg::IMemoryAllocator
    {
    public:
        static ObjectMemoryAllocator& Instance();

    public:
        void* Allocate(size_t size, const char*, const char*, const int) override;
        void  Free(void* ptr) override;
    };

    template<typename Ty>
    class ObjectAllocator : public Dg::MakeNewRCObj<Ty, ObjectMemoryAllocator>
    {
    public:
        using Base = Dg::MakeNewRCObj<Ty, ObjectMemoryAllocator>;

        ObjectAllocator(
            const char* sourceName = std::source_location::current().function_name(),
            const char* fileName   = std::source_location::current().file_name(),
            int         lineNumber = std::source_location::current().line()) noexcept :
            Base(ObjectMemoryAllocator::Instance(), sourceName, fileName, lineNumber)
        {
        }

        ObjectAllocator(
            IObject* owner) :
            Base(owner)
        {
        }
    };

    /// Template class implementing base functionality for an object
    template<typename BaseInterface>
    class BaseObject : public RefCountedObject<BaseInterface>
    {
    public:
        using base_type = RefCountedObject<BaseInterface>;

    public:
        template<typename... Args>
        explicit BaseObject(
            IReferenceCounters* counters,
            Args&&... args) noexcept(std::is_nothrow_constructible_v<base_type, IReferenceCounters*, Args...>) :
            base_type(counters, std::forward<Args>(args)...)
        {
        }

        void AME_METHOD(QueryInterface)(
            const INTERFACE_ID& iid,
            IObject**           outObject) override
        {
            if (outObject == nullptr)
                return;

            *outObject = nullptr;
            if (iid == IID_Unknown)
            {
                *outObject = this;
                (*outObject)->AddRef();
            }
        }
    };
} // namespace Ame

//

#define IMPLEMENT_QUERY_INTERFACE_DECL() \
    void AME_METHOD(QueryInterface)(     \
        const INTERFACE_ID& iid,         \
        IObject**           outObject) override

#define IMPLEMENT_QUERY_INTERFACE_HEAD(ClassName) \
    void ClassName::QueryInterface(               \
        const INTERFACE_ID& iid,                  \
        IObject**           outObject)

//

#define IMPLEMENT_QUERY_INTERFACE_SUBOJECTS2_BODY(InterfaceID1, InterfaceID2, ParentClassName, ...) \
    {                                                                                               \
        if (iid == InterfaceID1 || iid == InterfaceID2)                                             \
        {                                                                                           \
            *outObject = this;                                                                      \
            (*outObject)->AddRef();                                                                 \
        }                                                                                           \
        else                                                                                        \
        {                                                                                           \
            IObject* const subObjects[]{                                                            \
                __VA_ARGS__                                                                         \
            };                                                                                      \
                                                                                                    \
            for (auto subObject : subObjects)                                                       \
            {                                                                                       \
                if (subObject)                                                                      \
                {                                                                                   \
                    subObject->QueryInterface(iid, outObject);                                      \
                    if (*outObject)                                                                 \
                    {                                                                               \
                        return;                                                                     \
                    }                                                                               \
                }                                                                                   \
            }                                                                                       \
                                                                                                    \
            ParentClassName::QueryInterface(iid, outObject);                                        \
        }                                                                                           \
    }

#define IMPLEMENT_QUERY_INTERFACE_SUBOJECTS_BODY(InterfaceID, ParentClassName, ...) \
    {                                                                               \
        if (iid == InterfaceID)                                                     \
        {                                                                           \
            *outObject = this;                                                      \
            (*outObject)->AddRef();                                                 \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            IObject* const subObjects[]{                                            \
                __VA_ARGS__                                                         \
            };                                                                      \
                                                                                    \
            for (auto subObject : subObjects)                                       \
            {                                                                       \
                if (subObject)                                                      \
                {                                                                   \
                    subObject->QueryInterface(iid, outObject);                      \
                    if (*outObject)                                                 \
                    {                                                               \
                        return;                                                     \
                    }                                                               \
                }                                                                   \
            }                                                                       \
                                                                                    \
            ParentClassName::QueryInterface(iid, outObject);                        \
        }                                                                           \
    }

//

#define IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS(InterfaceID, ParentClassName, ...) \
    IMPLEMENT_QUERY_INTERFACE_DECL()                                                    \
    IMPLEMENT_QUERY_INTERFACE_SUBOJECTS_BODY(InterfaceID, ParentClassName, __VA_ARGS__)

#define IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(InterfaceID1, InterfaceID2, ParentClassName, ...) \
    IMPLEMENT_QUERY_INTERFACE_DECL()                                                                    \
    IMPLEMENT_QUERY_INTERFACE_SUBOJECTS2_BODY(InterfaceID1, InterfaceID2, ParentClassName, __VA_ARGS__)

//

#define IMPLEMENT_QUERY_INTERFACE_SUBOJECTS(InterfaceID, ParentClassName, ...) \
    IMPLEMENT_QUERY_INTERFACE_HEAD()                                           \
    IMPLEMENT_QUERY_INTERFACE_SUBOJECTS_BODY(InterfaceID, ParentClassName, __VA_ARGS__)

#define IMPLEMENT_QUERY_INTERFACE_SUBOJECTS2(InterfaceID1, InterfaceID2, ParentClassName, ...) \
    IMPLEMENT_QUERY_INTERFACE_HEAD()                                                           \
    IMPLEMENT_QUERY_INTERFACE_SUBOJECTS2_BODY(InterfaceID1, InterfaceID2, ParentClassName, __VA_ARGS__)
