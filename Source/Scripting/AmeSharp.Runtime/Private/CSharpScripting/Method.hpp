#pragma once

#include <Scripting/Method.hpp>
#include <Scripting/Types/NativeArray.hpp>
#include <CSharpScripting/Runtime.hpp>

namespace Ame::Scripting
{
    // {2FA2BB0D-7355-4AD4-81CE-DA38B0AFF923}
    inline constexpr UId IID_CSMethod{ 0x2fa2bb0d, 0x7355, 0x4ad4, { 0x81, 0xce, 0xda, 0x38, 0xb0, 0xaf, 0xf9, 0x23 } };

    class CSMethod : public BaseObject<IMethod>
    {
        using FreeFn          = void (*)(void* method);
        using IsStaticFn      = bool (*)(void* method);
        using GetNameFn       = NativeString (*)(void* method);
        using GetParamTypesFn = NativeArray<void*> (*)(void* method);
        using GetReturnTypeFn = void* (*)(void* method);
        using InvokeFn        = void (*)(void* method, IInstance* instance, void* const* arguments, size_t argCount,
                                  void* returnPtr);

    public:
        using Base = BaseObject<IMethod>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CSMethod, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(CSMethod, const CLRRuntime& runtime, void* method);

    public:
        ~CSMethod() override;

    public:
        auto IsStatic() const -> bool override;
        auto GetName() const -> NativeString override;

        auto GetParamTypes() const -> Co::generator<Ptr<IType>> override;
        auto GetReturnType() const -> Ptr<IType> override;
        void InvokeMethod(IInstance* instance, std::span<void* const> arguments, void* returnPtr) override;

    private:
        const CLRRuntime* m_Runtime = nullptr;
        void*             m_Method  = nullptr;
    };
} // namespace Ame::Scripting
