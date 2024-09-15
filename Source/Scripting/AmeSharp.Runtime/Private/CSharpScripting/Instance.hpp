#pragma once

#include <Scripting/Instance.hpp>
#include <CSharpScripting/Runtime.hpp>

namespace Ame::Scripting
{
    // {F53516D8-4B6B-4AEA-AADF-7892298F830A}
    inline constexpr UId IID_CSInstance{
        0xf53516d8, 0x4b6b, 0x4aea, { 0xaa, 0xdf, 0x78, 0x92, 0x29, 0x8f, 0x83, 0xa }
    };

    class CSType;

    class CSInstance : public BaseObject<IInstance>
    {
        using FreeFn         = void (*)(void*);
        using GetTypeFn      = void* (*)(void*);
        using InvokeMethodFn = void (*)(void*, const NativeString&, const void*, size_t, void*);
        using GetFieldFn     = void (*)(void*, const NativeString&, void*);
        using SetFieldFn     = void (*)(void*, const NativeString&, const void*);
        using GetPropertyFn  = void (*)(void*, const NativeString&, void*);
        using SetPropertyFn  = void (*)(void*, const NativeString&, const void*);

    public:
        using Base = BaseObject<IInstance>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CSInstance, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(CSInstance, CSType* type, void* instance);

    public:
        ~CSInstance() override;

    public:
        auto GetType() const -> IType* override;
        void InvokeMethod(const NativeString& methodName, std::span<void* const> arguments, void* returnPtr) override;

        void GetFieldMethod(const NativeString& fieldName, void* valuePtr) override;
        void SetFieldMethod(const NativeString& fieldName, const void* valuePtr) override;

        void GetPropertyMethod(const NativeString& propertyName, void* valuePtr) override;
        void SetPropertyMethod(const NativeString& propertyName, const void* valuePtr) override;

    public:
        [[nodiscard]] auto GetRuntime() const -> const CLRRuntime&;
        [[nodiscard]] auto GetHandle() const -> void*;

    private:
        Ptr<CSType> m_Type;
        void*       m_Instance = nullptr;
    };
} // namespace Ame::Scripting
