#pragma once

#include <source_location>
#include <DiligentCore/Common/interface/ObjectBase.hpp>
#include <Core/Interface.Macros.hpp>
#include <Core/UId.hpp>

namespace Dg = Diligent;

namespace Ame
{
    using IObject = Dg::IObject;

    using IReferenceCounters = Dg::IReferenceCounters;

    template<typename Ty> using RefCountedObject = Dg::RefCountedObject<Ty>;

    template<typename Ty> using Ptr = Dg::RefCntAutoPtr<Ty>;

    template<typename Ty> using WPtr = Dg::RefCntWeakPtr<Ty>;

    inline constexpr UId IID_Unknown = Dg::IID_Unknown;

    //

    class ObjectMemoryAllocator : public Dg::IMemoryAllocator
    {
    public:
        static ObjectMemoryAllocator& Instance()
        {
            static ObjectMemoryAllocator allocator;
            return allocator;
        }

    public:
        void* Allocate(size_t size, const char*, const char*, const int) override
        {
            return mi_malloc(size);
        }
        void Free(void* ptr) override
        {
            mi_free(ptr);
        }
    };

    template<typename Ty> class ObjectAllocator : public Dg::MakeNewRCObj<Ty, ObjectMemoryAllocator>
    {
    public:
        using Base = Dg::MakeNewRCObj<Ty, ObjectMemoryAllocator>;

        ObjectAllocator(const char* sourceName = std::source_location::current().function_name(),
                        const char* fileName   = std::source_location::current().file_name(),
                        int         lineNumber = std::source_location::current().line()) noexcept :
            Base(ObjectMemoryAllocator::Instance(), sourceName, fileName, lineNumber)
        {
        }

        ObjectAllocator(IObject* owner) : Base(owner)
        {
        }
    };

    //

    /// Template class implementing base functionality for an object
    template<typename BaseInterface> class BaseObject : public RefCountedObject<BaseInterface>
    {
    public:
        using base_type = RefCountedObject<BaseInterface>;

    public:
        template<typename... Args>
        explicit BaseObject(IReferenceCounters* counters, Args&&... args) noexcept(
            std::is_nothrow_constructible_v<base_type, IReferenceCounters*, Args...>) :
            base_type(counters, std::forward<Args>(args)...)
        {
        }

        void DILIGENT_CALL_TYPE QueryInterface(const INTERFACE_ID& iid, IObject** outObject) override
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

    //

    template<typename ObjectType, UId Id, typename BaseInterface = IObject>
        requires(!std::is_base_of_v<IObject, ObjectType> && std::is_base_of_v<IObject, BaseInterface>)
    class WrappedObject : public BaseObject<BaseInterface>, public ObjectType
    {
    public:
        using Base = BaseObject<BaseInterface>;

        template<typename... ArgsTy>
        WrappedObject(IReferenceCounters* counters, ArgsTy&&... args) noexcept(
            std::is_nothrow_constructible_v<BaseObject<BaseInterface>, IReferenceCounters*> &&
            std::is_nothrow_constructible_v<ObjectType, ArgsTy...>) :
            Base(counters), ObjectType(std::forward<ArgsTy>(args)...)
        {
        }

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(Id, IID_Unknown, Base);

    public:
        [[nodiscard]] ObjectType& Get() noexcept
        {
            return *this;
        }

        [[nodiscard]] const ObjectType& Get() const noexcept
        {
            return *this;
        }
    };
} // namespace Ame
