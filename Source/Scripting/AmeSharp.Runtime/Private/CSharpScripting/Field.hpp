#pragma once

#include <Scripting/Field.hpp>
#include <CSharpScripting/Runtime.hpp>

namespace Ame::Scripting
{
    // {D488DFC4-4301-45F4-A581-DE82C93AA9A5}
    inline constexpr UId IID_CSField{ 0xd488dfc4, 0x4301, 0x45f4, { 0xa5, 0x81, 0xde, 0x82, 0xc9, 0x3a, 0xa9, 0xa5 } };

    class CSType;

    class CSField : public BaseObject<IField>
    {
        using FreeFn     = void (*)(void*);
        using GetNameFn  = NativeString (*)(void*);
        using IsStaticFn = bool (*)(void*);
        using GetValueFn = void (*)(void*, void*, void*);
        using SetValueFn = void (*)(void*, void*, const void*);

    public:
        using Base = BaseObject<IField>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CSField, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(CSField, CSType* type, void* field);

    public:
        ~CSField() override;

    public:
        bool IsStatic() const override;
        auto GetName() const -> NativeString override;
        auto GetType() const -> IType* override;

        void GetValue(IInstance* instance, void* valuePtr) override;
        void SetValue(IInstance* instance, const void* valuePtr) override;

    public:
        [[nodiscard]] auto GetRuntime() const -> const CLRRuntime&;

    private:
        Ptr<CSType> m_Type;
        void*       m_Field;
    };
} // namespace Ame::Scripting
