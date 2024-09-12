#include <Interfaces/CSharpScripting/CSharpScriptEngine.hpp>
#include <CSharpScripting/Library.hpp>

#include <Log/Logger.hpp>

namespace Ame::Interfaces
{
    Ptr<IScriptEngine> CreateCSharpScriptingEngine(const Scripting::CSScriptEngineConfig& config)
    {
        return AmeCreate(CSharpScriptEngine, config);
    }

    CSharpScriptEngine::CSharpScriptEngine(IReferenceCounters*                    counters,
                                           const Scripting::CSScriptEngineConfig& config) :
        Base(counters), m_Runtime(config)
    {
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