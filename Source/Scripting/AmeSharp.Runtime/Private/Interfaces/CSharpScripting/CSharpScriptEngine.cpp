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
        IScriptEngine(counters), m_Runtime(config), m_Gc(AmeCreate(Scripting::CSGarbageCollector, m_Runtime))
    {
    }

    //

    Scripting::IGarbageCollector* CSharpScriptEngine::GetGarbageCollector()
    {
        return m_Gc;
    }

    Scripting::ILibraryContext* CSharpScriptEngine::CreateLibraryContext(const Scripting::NativeString& contextName)
    {
        return GetOrCreateLibraryContext(contextName);
    }

    void CSharpScriptEngine::TryUnloadLibraryContext(const Scripting::NativeString& name)
    {
        m_LibraryContexts.erase(name.hash());
    }

    Scripting::ILibrary* CSharpScriptEngine::CreateLibrary(const Scripting::NativeString& contextName,
                                                           const Scripting::NativeString& path)
    {
        auto context = GetOrCreateLibraryContext(contextName);
        return context->LoadLibrary(path);
    }

    //

    Scripting::CSLibraryContext* CSharpScriptEngine::GetOrCreateLibraryContext(const Scripting::NativeString& name)
    {
        auto it = m_LibraryContexts.find(name.hash());
        if (it == m_LibraryContexts.end())
        {
            it = m_LibraryContexts.emplace(name.hash(), AmeCreate(Scripting::CSLibraryContext, m_Runtime, name)).first;
        }
        return it->second;
    }
} // namespace Ame::Interfaces