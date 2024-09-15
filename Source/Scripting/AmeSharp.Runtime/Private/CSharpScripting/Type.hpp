#pragma once

#include <Scripting/Type.hpp>
#include <Scripting/Types/NativeArray.hpp>
#include <CSharpScripting/Runtime.hpp>

namespace Ame::Scripting
{
    // {B4494A29-5C6E-4121-AB0B-87A4764CF671}
    inline constexpr UId IID_CSType{ 0xb4494a29, 0x5c6e, 0x4121, { 0xab, 0xb, 0x87, 0xa4, 0x76, 0x4c, 0xf6, 0x71 } };

    class CSLibrary;

    class CSType : public BaseObject<IType>
    {
        using FreeFn           = void (*)(void* type);
        using GetNameFn        = NativeString (*)(void* type);
        using GetBaseTypeFn    = void* (*)(void* type);
        using CastAsFn         = bool (*)(void* type, void* otherType);
        using GetSizeFn        = size_t (*)(void* type);
        using CreateInstanceFn = void* (*)(void* type, void* const* args, size_t argsCount);
        using GetMethodFn      = void* (*)(void* type, const NativeString& name);
        using GetMethodsFn     = NativeArray<void*> (*)(void* type);
        using GetAttributeFn   = void* (*)(void* type, const NativeString& name);
        using GetAttributesFn  = NativeArray<void*> (*)(void* type);
        using GetPropertyFn    = void* (*)(void* type, const NativeString& name);
        using GetPropertiesFn  = NativeArray<void*> (*)(void* type);
        using GetFieldFn       = void* (*)(void* type, const NativeString& name);
        using GetFieldsFn      = NativeArray<void*> (*)(void* type);

    public:
        using Base = BaseObject<IType>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CSType, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(CSType, CSLibrary* library, void* type);

    public:
        ~CSType() override;

    public:
        auto GetLibrary() const -> ILibrary* override;
        auto GetName() const -> NativeString override;
        auto GetBaseType() const -> Ptr<IType> override;
        auto CastAs(IType* type) const -> bool override;
        auto GetSize() const -> size_t override;

        auto CreateInstanceRaw(std::span<void* const> args) -> Ptr<IInstance> override;

        auto GetField(const NativeString& name) -> Ptr<IField> override;
        auto GetMethod(const NativeString& name) -> Ptr<IMethod> override;
        auto GetAttribute(const NativeString& name) -> Ptr<IAttribute> override;
        auto GetProperty(const NativeString& name) -> Ptr<IProperty> override;

        auto GetFields() -> Co::generator<Ptr<IField>> override;
        auto GetMethods() -> Co::generator<Ptr<IMethod>> override;
        auto GetAttributes() -> Co::generator<Ptr<IAttribute>> override;
        auto GetProperties() -> Co::generator<Ptr<IProperty>> override;

    public:
        [[nodiscard]] auto GetRuntime() const -> const CLRRuntime&;
        [[nodiscard]] auto GetCSLibrary() const -> CSLibrary*;

    private:
        Ptr<CSLibrary> m_Library;
        void*          m_Type = nullptr;
    };
} // namespace Ame::Scripting
