#pragma once

#include <Interfaces/Scripting/ScriptEngine.hpp>

#include <CSharpScriptEngine/GC.hpp>
#include <CSharpScriptEngine/LibraryContext.hpp>

namespace Ame::Interfaces
{
    class CSharpScriptEngine : public IScriptEngine
    {
    public:
        auto GetGarbageCollector() -> Scripting::IGarbageCollector* override;

        auto CreateLibraryContext(const String& name) -> Scripting::ILibraryContext* override;
        auto CreateLibrary(const String& contextName, const String& path) -> Scripting::ILibrary* override;

    private:
        Ptr<Scripting::CSGarbageCollector> m_Gc;
        Ptr<Scripting::CSLibraryContext>   m_LibraryContext;
    };
} // namespace Ame::Interfaces