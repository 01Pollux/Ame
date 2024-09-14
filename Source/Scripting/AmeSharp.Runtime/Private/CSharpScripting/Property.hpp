#pragma once

#include <Core/Interface.hpp>
#include <Core/Coroutine.hpp>

namespace Ame::Scripting
{
    class IInstance;
    class IType;

    class IProperty
    {
    public:
        virtual auto GetName() const -> const char* = 0;
        virtual auto GetType() const -> IType*      = 0;

        /// If the field is static, the instance parameter is ignored
        virtual void GetValue(IInstance* instance, void* valuePtr) = 0;
        /// If the field is static, the instance parameter is ignored
        virtual void SetValue(IInstance* instance, const void* valuePtr) = 0;
    };
} // namespace Ame::Scripting
