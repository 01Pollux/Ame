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

    CSInstance::CSInstance(IReferenceCounters* counters, const CLRRuntime& runtime, void* instance) :
        Base(counters), m_Runtime(&runtime), m_Instance(instance)
    {
    }

    CSInstance::~CSInstance()
    {
        auto instanceFree = m_Runtime->GetCommonFunction<FreeFn>(CLRRuntime::Functions::InstanceBridge_Free);
        instanceFree(m_Instance);
    }

    void CSInstance::InvokeMethod(const NativeString& methodName, std::span<void* const> arguments, void* returnPtr)
    {
        auto instanceInvokeMethod =
            m_Runtime->GetCommonFunction<InvokeMethodFn>(CLRRuntime::Functions::InstanceBridge_InvokeMethod);
        instanceInvokeMethod(m_Instance, methodName, arguments.data(), arguments.size(), returnPtr);
    }

    void CSInstance::GetFieldMethod(const NativeString& fieldName, void* valuePtr)
    {
        auto instanceGetField =
            m_Runtime->GetCommonFunction<GetFieldFn>(CLRRuntime::Functions::InstanceBridge_GetField);
        instanceGetField(m_Instance, fieldName, valuePtr);
    }

    void CSInstance::SetFieldMethod(const NativeString& fieldName, const void* valuePtr)
    {
        auto instanceSetField =
            m_Runtime->GetCommonFunction<SetFieldFn>(CLRRuntime::Functions::InstanceBridge_SetField);
        instanceSetField(m_Instance, fieldName, valuePtr);
    }

    void CSInstance::GetPropertyMethod(const NativeString& propertyName, void* valuePtr)
    {
        auto instanceGetProperty =
            m_Runtime->GetCommonFunction<GetPropertyFn>(CLRRuntime::Functions::InstanceBridge_GetProperty);
        instanceGetProperty(m_Instance, propertyName, valuePtr);
    }

    void CSInstance::SetPropertyMethod(const NativeString& propertyName, const void* valuePtr)
    {
        auto instanceSetProperty =
            m_Runtime->GetCommonFunction<SetPropertyFn>(CLRRuntime::Functions::InstanceBridge_SetProperty);
        instanceSetProperty(m_Instance, propertyName, valuePtr);
    }
} // namespace Ame::Scripting
