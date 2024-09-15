#pragma once

#include <Core/Interface.hpp>
#include <Core/Coroutine.hpp>
#include <Scripting/Types/NativeString.hpp>
#include <Scripting/Types/Converter.hpp>

namespace Ame::Scripting
{
    class IInstance;
    class IType;

    class IMethod : public IObject
    {
    public:
        [[nodiscard]] virtual auto GetType() const -> IType*       = 0;
        [[nodiscard]] virtual auto IsStatic() const -> bool        = 0;
        [[nodiscard]] virtual auto GetName() const -> NativeString = 0;

        [[nodiscard]] virtual auto GetParamTypes() const -> Co::generator<Ptr<IType>>                     = 0;
        [[nodiscard]] virtual auto GetReturnType() const -> Ptr<IType>                                    = 0;
        virtual void InvokeMethod(IInstance* instance, std::span<void* const> arguments, void* returnPtr) = 0;
        void         InvokeStaticMethod(std::span<void* const> arguments, void* returnPtr)
        {
            InvokeMethod(nullptr, arguments, returnPtr);
        }

        template<typename RetTy, typename... Args> RetTy Invoke(IInstance* instance, Args&&... args)
        {
            return InvokeImpl<RetTy>(instance, NativeConverter<Args>::Wrap(std::forward<Args>(args))...);
        }

        template<typename RetTy, typename... Args> RetTy InvokeStatic(Args&&... args)
        {
            return InvokeImpl<RetTy>(nullptr, NativeConverter<Args>::Wrap(std::forward<Args>(args))...);
        }

    private:
        template<typename RetTy, typename... Args> RetTy InvokeImpl(IInstance* instance, Args&&... args)
        {
            constexpr size_t argsCount = sizeof...(args);

            if constexpr (argsCount == 0)
            {
                constexpr std::array<void*, 0> emptyArgs{};
                if constexpr (std::is_void_v<RetTy>)
                {
                    InvokeMethod(instance, emptyArgs, nullptr);
                }
                else
                {
                    RetTy result{};
                    InvokeMethod(instance, emptyArgs, &result);
                    return result;
                }
            }
            else
            {
                void* const argsPtrs[argsCount] = { std::addressof(args)... };
                if constexpr (std::is_void_v<RetTy>)
                {
                    InvokeMethod(instance, argsPtrs, nullptr);
                }
                else
                {
                    RetTy result{};
                    InvokeMethod(instance, argsPtrs, &result);
                    return result;
                }
            }
        }
    };
} // namespace Ame::Scripting
