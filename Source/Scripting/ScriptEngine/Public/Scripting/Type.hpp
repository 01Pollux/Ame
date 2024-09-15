#pragma once

#include <Core/Interface.hpp>
#include <Core/Coroutine.hpp>
#include <Scripting/Types/NativeString.hpp>
#include <Scripting/Types/Converter.hpp>

namespace Ame::Scripting
{
    class IInstance;
    class IField;
    class IMethod;
    class IAttribute;
    class IProperty;

    class IType : public IObject
    {
    public:
        [[nodiscard]] virtual auto GetName() const -> NativeString   = 0;
        [[nodiscard]] virtual auto GetBaseType() const -> Ptr<IType> = 0;
        [[nodiscard]] virtual auto CastAs(IType* type) const -> bool = 0;
        [[nodiscard]] virtual auto GetSize() const -> size_t         = 0;

        [[nodiscard]] virtual auto CreateInstanceRaw(std::span<void* const> args) -> Ptr<IInstance> = 0;

        [[nodiscard]] virtual auto GetField(const NativeString& name) -> IField*             = 0;
        [[nodiscard]] virtual auto GetMethod(const NativeString& name) -> Ptr<IMethod>       = 0;
        [[nodiscard]] virtual auto GetAttribute(const NativeString& name) -> Ptr<IAttribute> = 0;
        [[nodiscard]] virtual auto GetProperty(const NativeString& name) -> IProperty*       = 0;

        [[nodiscard]] virtual auto GetFields() -> Co::generator<IField*>             = 0;
        [[nodiscard]] virtual auto GetMethods() -> Co::generator<Ptr<IMethod>>       = 0;
        [[nodiscard]] virtual auto GetAttributes() -> Co::generator<Ptr<IAttribute>> = 0;
        [[nodiscard]] virtual auto GetProperties() -> Co::generator<IProperty*>      = 0;

        template<typename... Args> auto CreateInstance(Args&&... args)
        {
            return CreateInstanceImpl(NativeConverter<Args>::Wrap(std::forward<Args>(args))...);
        }

    private:
        template<typename... Args> auto CreateInstanceImpl(Args&&... args)
        {
            constexpr size_t argsCount = sizeof...(args);

            void* const argsPtrs[argsCount] = { std::addressof(args)... };
            return CreateInstanceRaw(argsPtrs);
        }
    };
} // namespace Ame::Scripting
