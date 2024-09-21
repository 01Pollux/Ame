#include <CSharpScripting/Instance.hpp>
#include <CSharpScripting/Type.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.Bridge.Core.Runtime.InstanceBridge, AmeSharp";

    void CLRRuntime::RegisterCommonFunctions_InstanceBridge()
    {
        RegisterCommonFunction(Functions::InstanceBridge_Free, GetFunctionPtr(ClassName, "Free"));
        RegisterCommonFunction(Functions::InstanceBridge_InvokeMethod, GetFunctionPtr(ClassName, "Invoke"));
        RegisterCommonFunction(Functions::InstanceBridge_GetField, GetFunctionPtr(ClassName, "GetField"));
        RegisterCommonFunction(Functions::InstanceBridge_SetField, GetFunctionPtr(ClassName, "SetField"));
        RegisterCommonFunction(Functions::InstanceBridge_GetProperty, GetFunctionPtr(ClassName, "GetProperty"));
        RegisterCommonFunction(Functions::InstanceBridge_SetProperty, GetFunctionPtr(ClassName, "SetProperty"));
    }

    CSInstance::CSInstance(IReferenceCounters* counters, CSType* type, void* instance) :
        IInstance(counters), m_Type(type), m_Instance(instance)
    {
    }

    CSInstance::~CSInstance()
    {
        auto instanceFree = GetRuntime().GetCommonFunction<FreeFn>(CLRRuntime::Functions::InstanceBridge_Free);
        instanceFree(m_Instance);
    }

    //

    IType* CSInstance::GetType() const
    {
        return m_Type;
    }

    void CSInstance::InvokeMethod(const NativeString& methodName, std::span<void* const> arguments, void* returnPtr)
    {
        auto instanceInvokeMethod =
            GetRuntime().GetCommonFunction<InvokeMethodFn>(CLRRuntime::Functions::InstanceBridge_InvokeMethod);
        instanceInvokeMethod(m_Instance, methodName, arguments.data(), arguments.size(), returnPtr);
    }

    void CSInstance::GetFieldMethod(const NativeString& fieldName, void* valuePtr)
    {
        auto instanceGetField =
            GetRuntime().GetCommonFunction<GetFieldFn>(CLRRuntime::Functions::InstanceBridge_GetField);
        instanceGetField(m_Instance, fieldName, valuePtr);
    }

    void CSInstance::SetFieldMethod(const NativeString& fieldName, const void* valuePtr)
    {
        auto instanceSetField =
            GetRuntime().GetCommonFunction<SetFieldFn>(CLRRuntime::Functions::InstanceBridge_SetField);
        instanceSetField(m_Instance, fieldName, valuePtr);
    }

    void CSInstance::GetPropertyMethod(const NativeString& propertyName, void* valuePtr)
    {
        auto instanceGetProperty =
            GetRuntime().GetCommonFunction<GetPropertyFn>(CLRRuntime::Functions::InstanceBridge_GetProperty);
        instanceGetProperty(m_Instance, propertyName, valuePtr);
    }

    void CSInstance::SetPropertyMethod(const NativeString& propertyName, const void* valuePtr)
    {
        auto instanceSetProperty =
            GetRuntime().GetCommonFunction<SetPropertyFn>(CLRRuntime::Functions::InstanceBridge_SetProperty);
        instanceSetProperty(m_Instance, propertyName, valuePtr);
    }

    const CLRRuntime& CSInstance::GetRuntime() const
    {
        return m_Type->GetRuntime();
    }

    auto CSInstance::GetHandle() const -> void*
    {
        return m_Instance;
    }
} // namespace Ame::Scripting
