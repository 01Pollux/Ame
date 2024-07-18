#pragma once

#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#include <DiligentCore/Common/interface/ObjectBase.hpp>

#include <source_location>

namespace Dg = Diligent;

#define AME_CALL_TYPE DILIGENT_CALL_TYPE
#define AME_METHOD(x) AME_CALL_TYPE x

namespace Ame
{
    using UId          = Dg::INTERFACE_ID;
    using INTERFACE_ID = Dg::INTERFACE_ID;

    using IObject = Dg::IObject;

    template<typename Ty>
    using BaseObject = Dg::ObjectBase<Ty>;

    using IReferenceCounters = Dg::IReferenceCounters;

    template<typename Ty>
    using RefCountedObject = Dg::RefCountedObject<Ty>;

    template<typename Ty>
    using Ptr = Dg::RefCntAutoPtr<Ty>;

    template<typename Ty>
    using WPtr = Dg::RefCntWeakPtr<Ty>;

    static constexpr UId IID_Unknown = Dg::IID_Unknown;

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
} // namespace Ame
