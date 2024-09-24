#include <CSharpScripting/Library.hpp>
#include <CSharpScripting/LibraryContext.hpp>
#include <CSharpScripting/Type.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.RuntimeHost.Runtime.AssemblyBridge, AmeSharp.RuntimeHost";

    void CLRRuntime::RegisterCommonFunctions_AssemblyBridge()
    {
        RegisterCommonFunction(Functions::AssemblyBridge_GetType, GetFunctionPtr(ClassName, "GetType"));
        RegisterCommonFunction(Functions::AssemblyBridge_GetTypes, GetFunctionPtr(ClassName, "GetTypes"));
    }

    //

    CSLibrary::CSLibrary(IReferenceCounters* counters, CSLibraryContext* context, void* library) :
        ILibrary(counters), m_Context(context), m_Library(library)
    {
    }

    auto CSLibrary::GetName() const -> NativeString
    {
        auto getName = GetRuntime().GetFunction<GetNameFn>(ClassName, "GetName");
        return getName(m_Library);
    }

    ILibraryContext* CSLibrary::GetContext() const
    {
        return m_Context;
    }

    Ptr<IType> CSLibrary::GetType(const NativeString& name)
    {
        auto getType = GetRuntime().GetCommonFunction<GetTypeFn>(CLRRuntime::Functions::AssemblyBridge_GetType);
        auto type    = getType(m_Library, name);
        return type ? AmeCreate(CSType, this, type) : Ptr<IType>{};
    }

    Co::generator<Ptr<IType>> CSLibrary::GetTypes()
    {
        auto getTypes = GetRuntime().GetCommonFunction<GetTypesFn>(CLRRuntime::Functions::AssemblyBridge_GetTypes);
        for (auto type : getTypes(m_Library))
        {
            co_yield AmeCreate(CSType, this, type);
        }
    }

    //

    const CLRRuntime& CSLibrary::GetRuntime() const
    {
        return m_Context->GetRuntime();
    }
} // namespace Ame::Scripting