#pragma once

#include <Scripting/Property.hpp>
#include <CSharpScripting/Runtime.hpp>

namespace Ame::Scripting
{
    // {87C5378E-EDA9-4962-98B5-258E64B608AE}
    inline constexpr UId IID_CSProperty{
        0x87c5378e, 0xeda9, 0x4962, { 0x98, 0xb5, 0x25, 0x8e, 0x64, 0xb6, 0x8, 0xae }
    };

    class CSType;

    class CSProperty : public BaseObject<IProperty>
    {
        using FreeFn     = void (*)(void*);
        using GetNameFn  = NativeString (*)(void*);
        using IsStaticFn = bool (*)(void*);
        using GetValueFn = void (*)(void*, void*, void*);
        using SetValueFn = void (*)(void*, void*, const void*);

    public:
        using Base = BaseObject<IProperty>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CSProperty, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(CSProperty, CSType* type, void* property);

    public:
        ~CSProperty() override;

    public:
        auto GetName() const -> NativeString override;
        auto GetType() const -> IType* override;
        bool IsStatic() const override;

        void GetValue(IInstance* instance, void* valuePtr) override;
        void SetValue(IInstance* instance, const void* valuePtr) override;

    public:
        [[nodiscard]] auto GetRuntime() const -> const CLRRuntime&;

    private:
        Ptr<CSType> m_Type;
        void*       m_Property;
    };
} // namespace Ame::Scripting
