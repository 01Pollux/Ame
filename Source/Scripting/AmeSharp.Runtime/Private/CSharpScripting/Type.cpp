#include <CSharpScripting/Type.hpp>
#include <CSharpScripting/Instance.hpp>
#include <CSharpScripting/Method.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.Bridge.Core.Runtime.TypeBridge, AmeSharp";

    void CLRRuntime::RegisterCommonFunctions_TypeBridge()
    {
        RegisterCommonFunction(Functions::TypeBridge_Free, GetFunctionPtr(ClassName, "Free"));
        RegisterCommonFunction(Functions::TypeBridge_GetName, GetFunctionPtr(ClassName, "GetName"));
        RegisterCommonFunction(Functions::TypeBridge_GetBaseType, GetFunctionPtr(ClassName, "GetBaseType"));
        RegisterCommonFunction(Functions::TypeBridge_GetMethod, GetFunctionPtr(ClassName, "GetMethod"));
        RegisterCommonFunction(Functions::TypeBridge_GetMethods, GetFunctionPtr(ClassName, "GetMethods"));
    }

    //

    CSType::CSType(IReferenceCounters* counters, const CLRRuntime& runtime, void* type) :
        Base(counters), m_Runtime(&runtime), m_Type(type)
    {
    }

    CSType::~CSType()
    {
        auto typeFree = m_Runtime->GetCommonFunction<FreeFn>(CLRRuntime::Functions::TypeBridge_Free);
        typeFree(m_Type);
    }

    //

    NativeString CSType::GetName() const
    {
        auto typeGetName = m_Runtime->GetCommonFunction<GetNameFn>(CLRRuntime::Functions::TypeBridge_GetName);
        return typeGetName(m_Type);
    }

    Ptr<IType> CSType::GetBaseType() const
    {
        auto typeGetBaseType =
            m_Runtime->GetCommonFunction<GetBaseTypeFn>(CLRRuntime::Functions::TypeBridge_GetBaseType);
        auto type = typeGetBaseType(m_Type);
        return type ? AmeCreate(CSType, *m_Runtime, type) : Ptr<IType>{};
    }

    bool CSType::CastAs(IType* type) const
    {
        (void)type;
        return {};
    }

    size_t CSType::GetSize() const
    {
        return {};
    }

    Ptr<IInstance> CSType::CreateInstance(const InstanceCreateDesc& createDesc)
    {
        (void)createDesc;
        return {};
    }

    IField* CSType::GetField(const NativeString& name)
    {
        (void)name;
        return {};
    }

    Ptr<IMethod> CSType::GetMethod(const NativeString& name)
    {
        auto getMethod = m_Runtime->GetCommonFunction<GetMethodFn>(CLRRuntime::Functions::TypeBridge_GetMethod);
        auto method    = getMethod(m_Type, name);
        return method ? AmeCreate(CSMethod, *m_Runtime, method) : Ptr<IMethod>{};
    }

    IProperty* CSType::GetProperty(const NativeString& name)
    {
        (void)name;
        return {};
    }

    Co::generator<IField*> CSType::GetFields()
    {
        co_return;
    }

    Co::generator<Ptr<IMethod>> CSType::GetMethods()
    {
        auto getMethods = m_Runtime->GetCommonFunction<GetMethodsFn>(CLRRuntime::Functions::TypeBridge_GetMethods);
        for (auto method : getMethods(m_Type))
        {
            co_yield AmeCreate(CSMethod, *m_Runtime, method);
        }
    }

    Co::generator<IProperty*> CSType::GetProperties()
    {
        co_return;
    }
} // namespace Ame::Scripting
