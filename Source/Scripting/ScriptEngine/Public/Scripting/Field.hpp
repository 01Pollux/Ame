#pragma once

#include <Core/Interface.hpp>
#include <Scripting/Types/NativeString.hpp>
#include <Scripting/Types/Converter.hpp>

namespace Ame::Scripting
{
    class IInstance;
    class IType;

    class IField : public IObject
    {
    public:
        virtual bool IsStatic() const                = 0;
        virtual auto GetName() const -> NativeString = 0;
        virtual auto GetType() const -> IType*       = 0;

        virtual void GetValue(IInstance* instance, void* valuePtr)       = 0;
        virtual void SetValue(IInstance* instance, const void* valuePtr) = 0;

    public:
        template<typename Ty> Ty Get(IInstance* instance)
        {
            Ty result{};
            GetValue(instance, std::addressof(result));
            return result;
        }

        template<typename Ty> void Set(IInstance* instance, Ty&& value)
        {
            SetImpl(instance, NativeConverter<Ty>::Wrap(std::forward<Ty>(value)));
        }

    private:
        template<typename Ty> void SetImpl(IInstance* instance, const Ty& value)
        {
            const void* valuePtr = std::bit_cast<const void*>(std::addressof(value));
            SetValue(instance, valuePtr);
        }
    };
} // namespace Ame::Scripting
