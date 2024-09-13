#pragma once

#include <map>

#include <Interfaces/Scripting/CSharpScriptEngine.hpp>
#include <CSharpScripting/Runtime.hpp>

#include <CSharpScripting/EngineConfig.hpp>
#include <CSharpScripting/GC.hpp>
#include <CSharpScripting/LibraryContext.hpp>

namespace Ame::Interfaces
{
    class CSharpScriptEngine : public BaseObject<IScriptEngine>
    {
    public:
        using Base = BaseObject<IScriptEngine>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CSScriptEngine, Base);

    private:
        using LibraryContextMap = std::map<size_t, Ptr<Scripting::CSLibraryContext>>;

    private:
        IMPLEMENT_INTERFACE_CTOR(CSharpScriptEngine, const Scripting::CSScriptEngineConfig& config);

    public:
        auto GetGarbageCollector() -> Scripting::IGarbageCollector* override;

        auto CreateLibraryContext(const Scripting::NativeString& name) -> Scripting::ILibraryContext* override;
        auto CreateLibrary(const Scripting::NativeString& contextName,
                           const Scripting::NativeString& path) -> Scripting::ILibrary* override;

    private:
        [[nodiscard]] Scripting::CSLibraryContext* GetOrCreateLibraryContext(const Scripting::NativeString& name);

    private:
        Scripting::CLRRuntime m_Runtime;

        Ptr<Scripting::CSGarbageCollector> m_Gc;
        LibraryContextMap                  m_LibraryContexts;
    };
} // namespace Ame::Interfaces