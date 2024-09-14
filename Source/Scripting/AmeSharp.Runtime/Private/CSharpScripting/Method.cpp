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

    CSMethod::CSMethod(IReferenceCounters* counters, const CLRRuntime& runtime, void* method) :
        Base(counters), m_Runtime(&runtime), m_Method(method)
    {
    }

    CSMethod::~CSMethod()
    {
        auto methodFree = m_Runtime->GetCommonFunction<FreeFn>(CLRRuntime::Functions::MethodBridge_Free);
        methodFree(m_Method);
    }

    //

    bool CSMethod::IsStatic() const
    {
        auto methodIsStatic = m_Runtime->GetCommonFunction<IsStaticFn>(CLRRuntime::Functions::MethodBridge_IsStatic);
        return methodIsStatic(m_Method);
    }

    NativeString CSMethod::GetName() const
    {
        auto methodGetName = m_Runtime->GetCommonFunction<GetNameFn>(CLRRuntime::Functions::MethodBridge_GetName);
        return methodGetName(m_Method);
    }

    Co::generator<Ptr<IType>> CSMethod::GetParamTypes() const
    {
        auto methodGetParamTypes =
            m_Runtime->GetCommonFunction<GetParamTypesFn>(CLRRuntime::Functions::MethodBridge_GetParamTypes);
        for (auto type : methodGetParamTypes(m_Method))
        {
            co_yield AmeCreate(CSType, *m_Runtime, type);
        }
    }

    Ptr<IType> CSMethod::GetReturnType() const
    {
        auto methodGetReturnType =
            m_Runtime->GetCommonFunction<GetReturnTypeFn>(CLRRuntime::Functions::MethodBridge_GetReturnType);
        auto type = methodGetReturnType(m_Method);
        return type ? AmeCreate(CSType, *m_Runtime, type) : Ptr<IType>{};
    }

    void CSMethod::InvokeMethod(IInstance* instance, std::span<void* const> arguments, size_t argCount, void* returnPtr)
    {
        auto methodInvoke = m_Runtime->GetCommonFunction<InvokeFn>(CLRRuntime::Functions::MethodBridge_Invoke);
        methodInvoke(m_Method, instance, arguments.data(), argCount, returnPtr);
    }
} // namespace Ame::Scripting