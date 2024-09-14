#pragma once

#include <Scripting/Type.hpp>
#include <Scripting/Types/NativeArray.hpp>
#include <CSharpScripting/Runtime.hpp>

namespace Ame::Scripting
{
    // {B4494A29-5C6E-4121-AB0B-87A4764CF671}
    inline constexpr UId IID_CSType{ 0xb4494a29, 0x5c6e, 0x4121, { 0xab, 0xb, 0x87, 0xa4, 0x76, 0x4c, 0xf6, 0x71 } };

    class CSType : public BaseObject<IType>
    {
        using FreeFn        = void (*)(void* type);
        using GetNameFn     = NativeString (*)(void* type);
        using GetBaseTypeFn = void* (*)(void* type);
        using GetMethodFn   = void* (*)(void* type, const NativeString& name);
        using GetMethodsFn  = NativeArray<void*> (*)(void* type);

    public:
        using Base = BaseObject<IType>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CSType, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(CSType, const CLRRuntime& runtime, void* type);

    public:
        ~CSType() override;

    public:
        auto GetName() const -> NativeString override;
        auto GetBaseType() const -> Ptr<IType> override;
        auto CastAs(IType* type) const -> bool override;
        auto GetSize() const -> size_t override;

        auto CreateInstance(const InstanceCreateDesc& createDesc) -> Ptr<IInstance> override;

        auto GetField(const NativeString& name) -> IField* override;
        auto GetMethod(const NativeString& name) -> Ptr<IMethod> override;
        auto GetProperty(const NativeString& name) -> IProperty* override;

        auto GetFields() -> Co::generator<IField*> override;
        auto GetMethods() -> Co::generator<Ptr<IMethod>> override;
        auto GetProperties() -> Co::generator<IProperty*> override;

    private:
        const CLRRuntime* m_Runtime = nullptr;
        void*             m_Type    = nullptr;
    };
} // namespace Ame::Scripting
