#include <CSharpScripting/Method.hpp>
#include <CSharpScripting/Type.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.Bridge.Core.Runtime.MethodBridge, AmeSharp";

    void CLRRuntime::RegisterCommonFunctions_MethodBridge()
    {
        RegisterCommonFunction(Functions::MethodBridge_Free, GetFunctionPtr(ClassName, "Free"));
        RegisterCommonFunction(Functions::MethodBridge_IsStatic, GetFunctionPtr(ClassName, "IsStatic"));
        RegisterCommonFunction(Functions::MethodBridge_GetName, GetFunctionPtr(ClassName, "GetName"));
        RegisterCommonFunction(Functions::MethodBridge_GetParamTypes, GetFunctionPtr(ClassName, "GetParamTypes"));
        RegisterCommonFunction(Functions::MethodBridge_GetReturnType, GetFunctionPtr(ClassName, "GetReturnType"));
        RegisterCommonFunction(Functions::MethodBridge_Invoke, GetFunctionPtr(ClassName, "Invoke"));
    }

    CSMethod::CSMethod(IReferenceCounters* counters, CSType* type, void* method) :
        Base(counters), m_Type(type), m_Method(method)
    {
    }

    CSMethod::~CSMethod()
    {
        auto methodFree = GetRuntime().GetCommonFunction<FreeFn>(CLRRuntime::Functions::MethodBridge_Free);
        methodFree(m_Method);
    }

    //

    Ptr<IType> CSMethod::GetType() const
    {
        return m_Type;
    }

    bool CSMethod::IsStatic() const
    {
        auto methodIsStatic = GetRuntime().GetCommonFunction<IsStaticFn>(CLRRuntime::Functions::MethodBridge_IsStatic);
        return methodIsStatic(m_Method);
    }

    NativeString CSMethod::GetName() const
    {
        auto methodGetName = GetRuntime().GetCommonFunction<GetNameFn>(CLRRuntime::Functions::MethodBridge_GetName);
        return methodGetName(m_Method);
    }

    Co::generator<Ptr<IType>> CSMethod::GetParamTypes() const
    {
        auto methodGetParamTypes =
            GetRuntime().GetCommonFunction<GetParamTypesFn>(CLRRuntime::Functions::MethodBridge_GetParamTypes);
        for (auto type : methodGetParamTypes(m_Method))
        {
            co_yield AmeCreate(CSType, m_Type->GetCSLibrary(), type);
        }
    }

    Ptr<IType> CSMethod::GetReturnType() const
    {
        auto methodGetReturnType =
            GetRuntime().GetCommonFunction<GetReturnTypeFn>(CLRRuntime::Functions::MethodBridge_GetReturnType);
        auto type = methodGetReturnType(m_Method);
        return type ? AmeCreate(CSType, m_Type->GetCSLibrary(), type) : Ptr<IType>{};
    }

    void CSMethod::InvokeMethod(IInstance* instance, std::span<void* const> arguments, void* returnPtr)
    {
        auto methodInvoke = GetRuntime().GetCommonFunction<InvokeFn>(CLRRuntime::Functions::MethodBridge_Invoke);
        methodInvoke(m_Method, instance, arguments.data(), arguments.size(), returnPtr);
    }

    //

    const CLRRuntime& CSMethod::GetRuntime() const
    {
        return m_Type->GetRuntime();
    }
} // namespace Ame::Scripting