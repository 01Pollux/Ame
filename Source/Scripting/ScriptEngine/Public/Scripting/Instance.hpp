#pragma once

#include <Core/Interface.hpp>
#include <Scripting/Types/NativeString.hpp>
#include <Scripting/Types/Converter.hpp>

namespace Ame::Scripting
{
    class IType;

    class IInstance : public IObject
    {
    public:
        virtual void InvokeMethod(const NativeString& methodName, std::span<void* const> arguments,
                                  void* returnPtr) = 0;

        virtual void GetFieldMethod(const NativeString& fieldName, void* valuePtr)       = 0;
        virtual void SetFieldMethod(const NativeString& fieldName, const void* valuePtr) = 0;

        virtual void GetPropertyMethod(const NativeString& propertyName, void* valuePtr)       = 0;
        virtual void SetPropertyMethod(const NativeString& propertyName, const void* valuePtr) = 0;

    public:
        template<typename RetTy, typename... Args> RetTy Invoke(const NativeString& methodName, Args&&... args)
        {
            return InvokeImpl<RetTy>(methodName, NativeConverter<Args>::Wrap(std::forward<Args>(args))...);
        }

        template<typename Ty> Ty GetField(const NativeString& fieldName)
        {
            Ty result{};
            GetFieldMethod(fieldName, std::addressof(result));
            return result;
        }

        template<typename Ty> void SetField(const NativeString& fieldName, Ty&& value)
        {
            SetFieldImpl(fieldName, NativeConverter<Ty>::Wrap(std::forward<Ty>(value)));
        }

        template<typename Ty> Ty GetProperty(const NativeString& propertyName)
        {
            Ty result{};
            GetPropertyMethod(propertyName, std::addressof(result));
            return result;
        }

        template<typename Ty> void SetProperty(const NativeString& propertyName, Ty&& value)
        {
            SetPropertyImpl(propertyName, NativeConverter<Ty>::Wrap(std::forward<Ty>(value)));
        }

    private:
        template<typename RetTy, typename... Args> RetTy InvokeImpl(const NativeString& methodName, Args&&... args)
        {
            constexpr size_t argsCount = sizeof...(args);

            if constexpr (argsCount == 0)
            {
                constexpr std::array<void*, 0> emptyArgs{};
                if constexpr (std::is_void_v<RetTy>)
                {
                    InvokeMethod(methodName, emptyArgs, nullptr);
                }
                else
                {
                    RetTy result{};
                    InvokeMethod(methodName, emptyArgs, &result);
                    return result;
                }
            }
            else
            {
                void* const argsPtrs[argsCount] = { std::addressof(args)... };
                if constexpr (std::is_void_v<RetTy>)
                {
                    InvokeMethod(methodName, argsPtrs, nullptr);
                }
                else
                {
                    RetTy result{};
                    InvokeMethod(methodName, argsPtrs, &result);
                    return result;
                }
            }
        }

        template<typename Ty> void SetFieldImpl(const NativeString& fieldName, const Ty& value)
        {
            const void* valuePtr = std::bit_cast<const void*>(std::addressof(value));
            SetFieldMethod(fieldName, valuePtr);
        }

        template<typename Ty> void SetPropertyImpl(const NativeString& fieldName, const Ty& value)
        {
            const void* valuePtr = std::bit_cast<const void*>(std::addressof(value));
            SetPropertyMethod(fieldName, valuePtr);
        }
    };
} // namespace Ame::Scripting
