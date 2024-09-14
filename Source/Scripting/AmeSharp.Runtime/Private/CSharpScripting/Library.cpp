#include <CSharpScripting/Library.hpp>
#include <CSharpScripting/LibraryContext.hpp>
#include <CSharpScripting/Type.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.Bridge.Core.Runtime.AssemblyBridge, AmeSharp";

    void CLRRuntime::RegisterCommonFunctions_AssemblyBridge()
    {
        RegisterCommonFunction(Functions::AssemblyBridge_GetType, GetFunctionPtr(ClassName, "GetType"));
        RegisterCommonFunction(Functions::AssemblyBridge_GetTypes, GetFunctionPtr(ClassName, "GetTypes"));
    }

    //

    CSLibrary::CSLibrary(IReferenceCounters* counters, const CLRRuntime& runtime, CSLibraryContext* context,
                         void* library) : Base(counters), m_Runtime(&runtime), m_Context(context), m_Library(library)
    {
    }

    auto CSLibrary::GetName() const -> NativeString
    {
        auto getName = m_Runtime->GetFunction<GetNameFn>(ClassName, "GetName");
        return getName(m_Library);
    }

    ILibraryContext* CSLibrary::GetContext() const
    {
        return m_Context;
    }

    Ptr<IType> CSLibrary::GetType(const NativeString& name)
    {
        auto getType = m_Runtime->GetCommonFunction<GetTypeFn>(CLRRuntime::Functions::AssemblyBridge_GetType);
        auto type    = getType(m_Library, name);
        return type ? AmeCreate(CSType, *m_Runtime, type) : Ptr<IType>{};
    }

    Co::generator<Ptr<IType>> CSLibrary::GetTypes()
    {
        auto getTypes = m_Runtime->GetCommonFunction<GetTypesFn>(CLRRuntime::Functions::AssemblyBridge_GetTypes);
        for (auto type : getTypes(m_Library))
        {
            co_yield AmeCreate(CSType, *m_Runtime, type);
        }
    }
} // namespace Ame::Scripting