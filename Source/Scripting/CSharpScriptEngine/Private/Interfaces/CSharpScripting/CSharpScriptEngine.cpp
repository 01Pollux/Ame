#include <Interfaces/CSharpScripting/CSharpScriptEngine.hpp>
#include <CSharpScripting/Library.hpp>

#include <nethost.h>
#include <hostfxr.h>
#include <Log/Logger.hpp>

namespace Ame::Interfaces
{
    Ptr<IScriptEngine> CreateCSharpScriptingEngine()
    {
        return AmeCreate(CSharpScriptEngine);
    }

    CSharpScriptEngine::CSharpScriptEngine(IReferenceCounters* counters) : Base(counters)
    {
        LoadHostFxrLibrary();
    }

    //

    Scripting::IGarbageCollector* CSharpScriptEngine::GetGarbageCollector()
    {
        return m_Gc;
    }

    Scripting::ILibraryContext* CSharpScriptEngine::CreateLibraryContext(const String& name)
    {
        return GetOrCreateLibraryContext(name);
    }

    Scripting::ILibrary* CSharpScriptEngine::CreateLibrary(const String& contextName, const String& path)
    {
        auto context = GetOrCreateLibraryContext(contextName);
        return context->LoadLibrary(path);
    }

    //

    void CSharpScriptEngine::LoadHostFxrLibrary()
    {
        size_t bufferSize = 0;
        get_hostfxr_path(nullptr, &bufferSize, nullptr);
        std::basic_string<char_t> hostFxrPath(bufferSize, '\0');
        if (get_hostfxr_path(hostFxrPath.data(), &bufferSize, nullptr))
        {
            throw std::runtime_error("Failed to locate hostfxr");
        }

        AME_LOG_TRACE(std::format("Found hostfxr at: {}", Strings::To<String>(hostFxrPath)));
        m_HostFxrLibrary = DllLibrary(hostFxrPath.c_str());
    }

    void CSharpScriptEngine::LoadHostFxrRuntime()
    {
        m_CoreClr.Load(m_HostFxrLibrary);
        m_CoreClr.SetErrorWriter([](const char_t* message) { AME_LOG_ERROR(Strings::To<String>(message)); });
    }

    //

    Scripting::CSLibraryContext* CSharpScriptEngine::GetOrCreateLibraryContext(const String& name)
    {
        auto it = m_LibraryContexts.find(name);
        if (it == m_LibraryContexts.end())
        {
            it = m_LibraryContexts.emplace(name, AmeCreate(Scripting::CSLibraryContext)).first;
        }
        return it->second;
    }
} // namespace Ame::Interfaces