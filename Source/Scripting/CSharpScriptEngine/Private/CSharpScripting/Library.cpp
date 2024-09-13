#include <CSharpScripting/Library.hpp>
#include <CSharpScripting/LibraryContext.hpp>
#include <CSharpScripting/Type.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.Bridge.Core.Runtime.AssemblyBridge, AmeSharp";

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
        (void)name;
        return {};
    }

    NativeArray<Ptr<IType>> CSLibrary::GetTypes()
    {
        //auto getTypes = m_Runtime->GetFunction<GetTypesFn>(ClassName, "GetTypes");
        return {};
    }
} // namespace Ame::Scripting