#pragma once

#include <Core/Interface.hpp>
#include <Core/Coroutine.hpp>

namespace Ame::Scripting
{
    class IInstance;
    class IType;

    class IMethod
    {
    public:
        virtual auto IsStatic() const -> bool       = 0;
        virtual auto GetName() const -> const char* = 0;

        virtual auto GetReturnType() const -> IType*                                     = 0;
        virtual void Invoke(IInstance* instance, const void* arguments, void* returnPtr) = 0;
    };
} // namespace Ame::Scripting
