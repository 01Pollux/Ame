#include <CSharpScripting/LibraryContext.hpp>
#include <CSharpScripting/Library.hpp>

namespace Ame::Scripting
{
    static constexpr const char* ClassName = "AmeSharp.RuntimeHost.Runtime.ALCBridge, AmeSharp.RuntimeHost";

    CSLibraryContext::CSLibraryContext(IReferenceCounters* counters, const CLRRuntime& runtime,
                                       const NativeString& name) : ILibraryContext(counters), m_Runtime(&runtime)
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
        if (auto library = GetLibrary(path); library != nullptr)
        {
            return library;
        }

        auto loadLibrary = m_Runtime->GetFunction<LoadLibraryFn>(ClassName, "LoadLibrary");
        auto library     = loadLibrary(m_Context, path);
        return m_Libraries.emplace(path.hash(), AmeCreate(CSLibrary, this, library)).first->second;
    }

    ILibrary* CSLibraryContext::LoadLibrary(const NativeString& name, const std::byte* data, size_t dataSize)
    {
        if (auto library = GetLibrary(name); library != nullptr)
        {
            return library;
        }

        auto loadLibraryFromStream =
            m_Runtime->GetFunction<LoadLibraryFromStreamFn>(ClassName, "LoadLibraryFromStream");
        auto library = loadLibraryFromStream(m_Context, data, dataSize);
        return m_Libraries.emplace(name.hash(), AmeCreate(CSLibrary, this, library)).first->second;
    }

    ILibrary* CSLibraryContext::GetLibrary(const NativeString& name)
    {
        auto iter = m_Libraries.find(name.hash());
        return iter != m_Libraries.end() ? iter->second : nullptr;
    }
} // namespace Ame::Scripting
