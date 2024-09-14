#pragma once

#include <Scripting/Instance.hpp>

namespace Ame::Scripting
{
    class IType;

    class CSInstance : public IInstance
    {
    public:
        auto GetType() const -> IType*
        {
            return {};
        }
        void InvokeMethod(const char* methodName, const void* arguments, void* returnPtr)
        {
            (void)methodName;
            (void)arguments;
            (void)returnPtr;
        }
        void GetField(const char* fieldName, void* valuePtr)
        {
            (void)fieldName;
            (void)valuePtr;
        }
        void SetField(const char* fieldName, const void* valuePtr)
        {
            fieldName;
            (void)valuePtr;
        }
        void GetProperty(const char* propertyName, void* valuePtr)
        {
            (void)propertyName;
            (void)valuePtr;
        }
        void SetProperty(const char* propertyName, const void* valuePtr)
        {
            (void)propertyName;
            (void)valuePtr;
        }
    };
} // namespace Ame::Scripting
