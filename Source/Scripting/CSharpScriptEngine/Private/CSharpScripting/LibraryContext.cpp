#include <CSharpScripting/LibraryContext.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.Bridge.Core.Runtime.ALCBridge, AmeSharp";

    CSLibraryContext::CSLibraryContext(IReferenceCounters* counters, const CLRRuntime& runtime,
                                       const NativeString& name) : Base(counters), m_Runtime(&runtime)
    {
        auto create = m_Runtime->GetFunction<CreateFn>(ClassName, "Create");
        m_Context   = create(name);
    }

    CSLibraryContext::~CSLibraryContext()
    {
        auto unload = m_Runtime->GetFunction<UnloadFn>(ClassName, "Unload");
        unload(m_Context);
    }

    ILibrary* CSLibraryContext::LoadLibrary(const NativeString& path)
    {
        (void)path;
        return nullptr;
    }

    ILibrary* CSLibraryContext::LoadLibrary(const NativeString& path, const std::byte* data, size_t dataSize)
    {
        (void)path;
        (void)data;
        (void)dataSize;
        return nullptr;
    }

    ILibrary* CSLibraryContext::GetLibrary(const NativeString& path)
    {
        (void)path;
        return nullptr;
    }
} // namespace Ame::Scripting
