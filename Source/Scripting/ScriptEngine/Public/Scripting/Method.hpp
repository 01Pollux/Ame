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
        virtual auto IsStatic() const -> bool        = 0;
        virtual auto GetName() const -> NativeString = 0;

        virtual auto GetParamTypes() const -> Co::generator<Ptr<IType>>                                   = 0;
        virtual auto GetReturnType() const -> Ptr<IType>                                                  = 0;
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
    };
} // namespace Ame::Scripting
