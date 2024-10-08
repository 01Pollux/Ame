#pragma once

#include <Scripting/Attribute.hpp>
#include <Scripting/Types/NativeArray.hpp>
#include <CSharpScripting/Runtime.hpp>

namespace Ame::Scripting
{
    // {BA590673-CA2C-47D9-B117-8395C81F1D0C}
    inline constexpr UId IID_CSAttribute{
        0xba590673, 0xca2c, 0x47d9, { 0xb1, 0x17, 0x83, 0x95, 0xc8, 0x1f, 0x1d, 0xc }
    };

    class CSType;

    class CSAttribute : public IAttribute
    {
        using FreeFn     = void (*)(void* attribute);
        using GetTypeFn  = void* (*)(void* attribute);
        using GetValueFn = void (*)(void* attribute, const NativeString& name, void* valuePtr);

    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CSAttribute, IAttribute);

    private:
        IMPLEMENT_INTERFACE_CTOR(CSAttribute, CSType* type, void* attribute);

    public:
        ~CSAttribute() override;

    public:
        auto GetType() const -> IType* override;
        void GetValue(const NativeString& name, void* const valuePtr) override;

    public:
        [[nodiscard]] auto GetRuntime() const -> const CLRRuntime&;

    private:
        Ptr<CSType> m_Type;
        void*       m_Attribute = nullptr;
    };
} // namespace Ame::Scripting
