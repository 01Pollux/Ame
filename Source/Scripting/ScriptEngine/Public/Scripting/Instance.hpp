#pragma once

#include <Core/Interface.hpp>
#include <Core/Coroutine.hpp>

namespace Ame::Scripting
{
    class IType;

    class IInstance : public IObject
    {
    public:
        virtual auto GetType() const -> IType* = 0;

        virtual void InvokeMethod(const char* methodName, const void* arguments, void* returnPtr) = 0;

        virtual void GetField(const char* fieldName, void* valuePtr)       = 0;
        virtual void SetField(const char* fieldName, const void* valuePtr) = 0;

        virtual void GetProperty(const char* propertyName, void* valuePtr)       = 0;
        virtual void SetProperty(const char* propertyName, const void* valuePtr) = 0;
    };
} // namespace Ame::Scripting
