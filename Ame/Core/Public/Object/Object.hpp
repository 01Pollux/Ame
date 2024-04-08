#pragma once

#include <Core/Ame.hpp>
#include <Object/TypeInfo.hpp>
#include <Object/Signal.hpp>

#define AME_OBJECT(Name, Base)                                                        \
public:                                                                               \
    using ThisClass = Name;                                                           \
    using BaseClass = Base;                                                           \
                                                                                      \
    [[nodiscard]] const Ame::TypeInfo& GetTypeInfo() const override                   \
    {                                                                                 \
        static const Ame::TypeInfo s_TypeInfo(#Name, BaseClass::GetTypeInfoStatic()); \
        return s_TypeInfo;                                                            \
    }                                                                                 \
    [[nodiscard]] static const Ame::TypeInfo& GetTypeInfoStatic()                     \
    {                                                                                 \
        static const Ame::TypeInfo s_TypeInfo(#Name);                                 \
        return s_TypeInfo;                                                            \
    }                                                                                 \
                                                                                      \
private:

namespace Ame
{
    class IObject : public NonCopyable, public NonMovable
    {
    public:
        virtual ~IObject() = default;

        [[nodiscard]] virtual const TypeInfo& GetTypeInfo() const = 0;
        [[nodiscard]] static const TypeInfo*  GetTypeInfoStatic()
        {
            return nullptr;
        }
        template<typename Ty>
        [[nodiscard]] Ty* As() const
        {
            return dynamic_cast<Ty*>(this);
        }
    };
} // namespace Ame