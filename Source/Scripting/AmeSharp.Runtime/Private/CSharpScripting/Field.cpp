#include <CSharpScripting/Field.hpp>
#include <CSharpScripting/Type.hpp>
#include <CSharpScripting/Instance.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.RuntimeHost.Runtime.FieldBridge, AmeSharp.RuntimeHost";

    void CLRRuntime::RegisterCommonFunctions_FieldBridge()
    {
        RegisterCommonFunction(Functions::FieldBridge_Free, GetFunctionPtr(ClassName, "Free"));
        RegisterCommonFunction(Functions::FieldBridge_GetName, GetFunctionPtr(ClassName, "GetName"));
        RegisterCommonFunction(Functions::FieldBridge_IsStatic, GetFunctionPtr(ClassName, "IsStatic"));
        RegisterCommonFunction(Functions::FieldBridge_GetValue, GetFunctionPtr(ClassName, "GetValue"));
        RegisterCommonFunction(Functions::FieldBridge_SetValue, GetFunctionPtr(ClassName, "SetValue"));
    }

    CSField::CSField(IReferenceCounters* counters, CSType* type, void* property) :
        IField(counters), m_Type(type), m_Field(property)
    {
    }

    CSField::~CSField()
    {
        auto fieldFree = GetRuntime().GetCommonFunction<FreeFn>(CLRRuntime::Functions::FieldBridge_Free);
        fieldFree(m_Field);
    }

    //

    NativeString CSField::GetName() const
    {
        auto getName = GetRuntime().GetCommonFunction<GetNameFn>(CLRRuntime::Functions::FieldBridge_GetName);
        return getName(m_Field);
    }

    IType* CSField::GetType() const
    {
        return m_Type;
    }

    bool CSField::IsStatic() const
    {
        auto isStatic = GetRuntime().GetCommonFunction<IsStaticFn>(CLRRuntime::Functions::FieldBridge_IsStatic);
        return isStatic(m_Field);
    }

    void CSField::GetValue(IInstance* instance, void* valuePtr)
    {
        auto getValue       = GetRuntime().GetCommonFunction<GetValueFn>(CLRRuntime::Functions::FieldBridge_GetValue);
        auto instanceHandle = instance ? static_cast<CSInstance*>(instance)->GetHandle() : nullptr;
        getValue(m_Field, instanceHandle, valuePtr);
    }

    void CSField::SetValue(IInstance* instance, const void* valuePtr)
    {
        auto setValue       = GetRuntime().GetCommonFunction<SetValueFn>(CLRRuntime::Functions::FieldBridge_SetValue);
        auto instanceHandle = instance ? static_cast<CSInstance*>(instance)->GetHandle() : nullptr;
        setValue(m_Field, instanceHandle, valuePtr);
    }

    //

    const CLRRuntime& CSField::GetRuntime() const
    {
        return m_Type->GetRuntime();
    }
} // namespace Ame::Scripting