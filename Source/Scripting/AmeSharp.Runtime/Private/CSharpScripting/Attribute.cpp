#include <CSharpScripting/Attribute.hpp>
#include <CSharpScripting/Type.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.Bridge.Core.Runtime.AttributeBridge, AmeSharp";

    void CLRRuntime::RegisterCommonFunctions_AttributeBridge()
    {
        RegisterCommonFunction(Functions::AttributeBridge_Free, GetFunctionPtr(ClassName, "Free"));
        RegisterCommonFunction(Functions::AttributeBridge_GetValue, GetFunctionPtr(ClassName, "GetValue"));
    }

    CSAttribute::CSAttribute(IReferenceCounters* counters, CSType* type, void* attribute) :
        Base(counters), m_Type(type), m_Attribute(attribute)
    {
    }

    CSAttribute::~CSAttribute()
    {
        auto attributeFree = GetRuntime().GetCommonFunction<FreeFn>(CLRRuntime::Functions::AttributeBridge_Free);
        attributeFree(m_Attribute);
    }

    //

    Ptr<IType> CSAttribute::GetType() const
    {
        return m_Type;
    }

    void CSAttribute::GetValue(const NativeString& name, void* const valuePtr)
    {
        auto attributeGetValue =
            GetRuntime().GetCommonFunction<GetValueFn>(CLRRuntime::Functions::AttributeBridge_GetValue);
        attributeGetValue(m_Attribute, name, valuePtr);
    }

    //

    const CLRRuntime& CSAttribute::GetRuntime() const
    {
        return m_Type->GetRuntime();
    }
} // namespace Ame::Scripting