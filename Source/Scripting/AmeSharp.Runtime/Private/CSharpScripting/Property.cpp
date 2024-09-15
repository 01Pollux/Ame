#include <CSharpScripting/Property.hpp>
#include <CSharpScripting/Type.hpp>
#include <CSharpScripting/Instance.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.Bridge.Core.Runtime.PropertyBridge, AmeSharp";

    void CLRRuntime::RegisterCommonFunctions_PropertyBridge()
    {
        RegisterCommonFunction(Functions::PropertyBridge_Free, GetFunctionPtr(ClassName, "Free"));
        RegisterCommonFunction(Functions::PropertyBridge_GetName, GetFunctionPtr(ClassName, "GetName"));
        RegisterCommonFunction(Functions::PropertyBridge_IsStatic, GetFunctionPtr(ClassName, "IsStatic"));
        RegisterCommonFunction(Functions::PropertyBridge_GetValue, GetFunctionPtr(ClassName, "GetValue"));
        RegisterCommonFunction(Functions::PropertyBridge_SetValue, GetFunctionPtr(ClassName, "SetValue"));
    }

    CSProperty::CSProperty(IReferenceCounters* counters, CSType* type, void* property) :
        Base(counters), m_Type(type), m_Property(property)
    {
    }

    CSProperty::~CSProperty()
    {
        auto propFree = GetRuntime().GetCommonFunction<FreeFn>(CLRRuntime::Functions::PropertyBridge_Free);
        propFree(m_Property);
    }

    //

    NativeString CSProperty::GetName() const
    {
        auto getName = GetRuntime().GetCommonFunction<GetNameFn>(CLRRuntime::Functions::PropertyBridge_GetName);
        return getName(m_Property);
    }

    IType* CSProperty::GetType() const
    {
        return m_Type;
    }

    bool CSProperty::IsStatic() const
    {
        auto isStatic = GetRuntime().GetCommonFunction<IsStaticFn>(CLRRuntime::Functions::PropertyBridge_IsStatic);
        return isStatic(m_Property);
    }

    void CSProperty::GetValue(IInstance* instance, void* valuePtr)
    {
        auto getValue = GetRuntime().GetCommonFunction<GetValueFn>(CLRRuntime::Functions::PropertyBridge_GetValue);
        auto instanceHandle = instance ? static_cast<CSInstance*>(instance)->GetHandle() : nullptr;
        getValue(m_Property, instanceHandle, valuePtr);
    }

    void CSProperty::SetValue(IInstance* instance, const void* valuePtr)
    {
        auto setValue = GetRuntime().GetCommonFunction<SetValueFn>(CLRRuntime::Functions::PropertyBridge_SetValue);
        auto instanceHandle = instance ? static_cast<CSInstance*>(instance)->GetHandle() : nullptr;
        setValue(m_Property, instanceHandle, valuePtr);
    }

    //

    const CLRRuntime& CSProperty::GetRuntime() const
    {
        return m_Type->GetRuntime();
    }
} // namespace Ame::Scripting