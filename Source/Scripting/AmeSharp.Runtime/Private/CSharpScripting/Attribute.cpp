#include <CSharpScripting/Attribute.hpp>
#include <CSharpScripting/Type.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.Bridge.Core.Runtime.AttributeBridge, AmeSharp";

    void CLRRuntime::RegisterCommonFunctions_AttributeBridge()
    {
        RegisterCommonFunction(Functions::AttributeBridge_Free, GetFunctionPtr(ClassName, "Free"));
        RegisterCommonFunction(Functions::AttributeBridge_GetType, GetFunctionPtr(ClassName, "GetType"));
        RegisterCommonFunction(Functions::AttributeBridge_GetValue, GetFunctionPtr(ClassName, "GetValue"));
    }

    CSAttribute::CSAttribute(IReferenceCounters* counters, const CLRRuntime& runtime, void* attribute) :
        Base(counters), m_Runtime(&runtime), m_Attribute(attribute)
    {
    }

    CSAttribute::~CSAttribute()
    {
        auto attributeFree = m_Runtime->GetCommonFunction<FreeFn>(CLRRuntime::Functions::AttributeBridge_Free);
        attributeFree(m_Attribute);
    }

    //

    Ptr<IType> CSAttribute::GetType() const
    {
        auto attributeGetType = m_Runtime->GetCommonFunction<GetTypeFn>(CLRRuntime::Functions::AttributeBridge_GetType);
        auto type             = attributeGetType(m_Attribute);
        return AmeCreate(CSType, *m_Runtime, type);
    }

    void CSAttribute::GetValue(const NativeString& name, void* const valuePtr)
    {
        auto attributeGetValue =
            m_Runtime->GetCommonFunction<GetValueFn>(CLRRuntime::Functions::AttributeBridge_GetValue);
        attributeGetValue(m_Attribute, name, valuePtr);
    }
} // namespace Ame::Scripting