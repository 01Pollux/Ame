#include <CSharpScripting/Type.hpp>
#include <CSharpScripting/Library.hpp>
#include <CSharpScripting/Instance.hpp>
#include <CSharpScripting/Method.hpp>
#include <CSharpScripting/Attribute.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.Bridge.Core.Runtime.TypeBridge, AmeSharp";

    void CLRRuntime::RegisterCommonFunctions_TypeBridge()
    {
        RegisterCommonFunction(Functions::TypeBridge_Free, GetFunctionPtr(ClassName, "Free"));
        RegisterCommonFunction(Functions::TypeBridge_GetName, GetFunctionPtr(ClassName, "GetName"));
        RegisterCommonFunction(Functions::TypeBridge_GetBaseType, GetFunctionPtr(ClassName, "GetBaseType"));
        RegisterCommonFunction(Functions::TypeBridge_CastAs, GetFunctionPtr(ClassName, "CastAs"));
        RegisterCommonFunction(Functions::TypeBridge_GetSize, GetFunctionPtr(ClassName, "GetSize"));
        RegisterCommonFunction(Functions::TypeBridge_CreateInstance, GetFunctionPtr(ClassName, "CreateInstance"));
        RegisterCommonFunction(Functions::TypeBridge_GetMethod, GetFunctionPtr(ClassName, "GetMethod"));
        RegisterCommonFunction(Functions::TypeBridge_GetMethods, GetFunctionPtr(ClassName, "GetMethods"));
        RegisterCommonFunction(Functions::TypeBridge_GetAttribute, GetFunctionPtr(ClassName, "GetAttribute"));
        RegisterCommonFunction(Functions::TypeBridge_GetAttributes, GetFunctionPtr(ClassName, "GetAttributes"));
    }

    //

    CSType::CSType(IReferenceCounters* counters, CSLibrary* library, void* type) :
        Base(counters), m_Library(library), m_Type(type)
    {
    }

    CSType::~CSType()
    {
        auto typeFree = GetRuntime().GetCommonFunction<FreeFn>(CLRRuntime::Functions::TypeBridge_Free);
        typeFree(m_Type);
    }

    //

    ILibrary* CSType::GetLibrary() const
    {
        return m_Library;
    }

    NativeString CSType::GetName() const
    {
        auto typeGetName = GetRuntime().GetCommonFunction<GetNameFn>(CLRRuntime::Functions::TypeBridge_GetName);
        return typeGetName(m_Type);
    }

    Ptr<IType> CSType::GetBaseType() const
    {
        auto typeGetBaseType =
            GetRuntime().GetCommonFunction<GetBaseTypeFn>(CLRRuntime::Functions::TypeBridge_GetBaseType);
        auto type = typeGetBaseType(m_Type);
        return type ? AmeCreate(CSType, m_Library, type) : Ptr<IType>{};
    }

    bool CSType::CastAs(IType* type) const
    {
        auto typeCastAs = GetRuntime().GetCommonFunction<CastAsFn>(CLRRuntime::Functions::TypeBridge_CastAs);
        return typeCastAs(m_Type, type);
    }

    size_t CSType::GetSize() const
    {
        auto typeGetSize = GetRuntime().GetCommonFunction<GetSizeFn>(CLRRuntime::Functions::TypeBridge_GetSize);
        return typeGetSize(m_Type);
    }

    Ptr<IInstance> CSType::CreateInstanceRaw(std::span<void* const> args)
    {
        auto createInstance =
            GetRuntime().GetCommonFunction<CreateInstanceFn>(CLRRuntime::Functions::TypeBridge_CreateInstance);
        auto instance = createInstance(m_Type, args.data(), args.size());
        return AmeCreate(CSInstance, this, instance);
    }

    //

    IField* CSType::GetField(const NativeString& name)
    {
        (void)name;
        return {};
    }

    Ptr<IMethod> CSType::GetMethod(const NativeString& name)
    {
        auto getMethod = GetRuntime().GetCommonFunction<GetMethodFn>(CLRRuntime::Functions::TypeBridge_GetMethod);
        auto method    = getMethod(m_Type, name);
        return method ? AmeCreate(CSMethod, this, method) : Ptr<IMethod>{};
    }

    Ptr<IAttribute> CSType::GetAttribute(const NativeString& name)
    {
        auto getAttribute =
            GetRuntime().GetCommonFunction<GetAttributeFn>(CLRRuntime::Functions::TypeBridge_GetAttribute);
        auto attribute = getAttribute(m_Type, name);
        return attribute ? AmeCreate(CSAttribute, this, attribute) : Ptr<IAttribute>{};
    }

    IProperty* CSType::GetProperty(const NativeString& name)
    {
        (void)name;
        return {};
    }

    //

    Co::generator<IField*> CSType::GetFields()
    {
        co_return;
    }

    Co::generator<Ptr<IMethod>> CSType::GetMethods()
    {
        auto getMethods = GetRuntime().GetCommonFunction<GetMethodsFn>(CLRRuntime::Functions::TypeBridge_GetMethods);
        for (auto method : getMethods(m_Type))
        {
            co_yield AmeCreate(CSMethod, this, method);
        }
    }

    Co::generator<Ptr<IAttribute>> CSType::GetAttributes()
    {
        auto getAttributes =
            GetRuntime().GetCommonFunction<GetAttributesFn>(CLRRuntime::Functions::TypeBridge_GetAttributes);
        for (auto attribute : getAttributes(m_Type))
        {
            co_yield AmeCreate(CSAttribute, this, attribute);
        }
    }

    Co::generator<IProperty*> CSType::GetProperties()
    {
        co_return;
    }

    const CLRRuntime& CSType::GetRuntime() const
    {
        return m_Library->GetRuntime();
    }

    auto CSType::GetCSLibrary() const -> CSLibrary*
    {
        return m_Library;
    }
} // namespace Ame::Scripting
