#pragma once

#include <Core/Interface.hpp>
#include <Scripting/Types/NativeString.hpp>

namespace Ame::Scripting
{
    class IType;

    class IAttribute : public IObject
    {
    public:
        [[nodiscard]] virtual auto GetType() const -> IType*                                = 0;
        [[nodiscard]] virtual void GetValue(const NativeString& name, void* const valuePtr) = 0;

        template<typename Ty> [[nodiscard]] Ty GetValue(const NativeString& name)
        {
            Ty value{};
            GetValue(name, std::addressof(value));
            return value;
        }

        template<> [[nodiscard]] String GetValue(const NativeString& name)
        {
            return GetValue<NativeString>(name).str();
        }
    };
} // namespace Ame::Scripting
