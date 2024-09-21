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

    template<typename Ty> using Ptr = Dg::RefCntAutoPtr<Ty>;

    template<typename Ty> using WPtr = Dg::RefCntWeakPtr<Ty>;

    inline constexpr UId IID_Unknown = Dg::IID_Unknown;

    //

    class AME_ENGINE_API ObjectMemoryAllocator : public Dg::IMemoryAllocator
    {
    public:
        static ObjectMemoryAllocator& Instance();

    public:
        void* Allocate(size_t size, const char*, const char*, const int) override;
        void  Free(void* ptr) override;
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

    class IObjectWithCallback : public Dg::ObjectBase<IObject>
    {
    public:
        using QueryCallback = IObject*(AME_CDECL*)(IObject* objectHandle, const UId* iidHandle);

        IMPLEMENT_INTERFACE_CTOR(IObjectWithCallback) : ObjectBase(counters)
        {
        }

        void AME_CDECL QueryInterface(const UId& iid, IObject** iface) override
        {
            if (iface == nullptr) [[unlikely]]
            {
                return;
            }

            if (QueryInterfaceCallback)
            {
                auto obj = QueryInterfaceCallback(this, &iid);
                if (obj)
                {
                    *iface = std::bit_cast<IObject*>(obj);
                    return;
                }
            }

            if (iid == IID_Unknown)
            {
                *iface = this;
                (*iface)->AddRef();
            }
        }

    public:
        QueryCallback QueryInterfaceCallback = nullptr;
    };
} // namespace Ame
