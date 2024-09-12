#pragma once

#include <map>

#include <CSharpScripting/Core.hpp>
#include <Interfaces/Scripting/CSharpScriptEngine.hpp>

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
        using LibraryContextMap = std::map<String, Ptr<Scripting::CSLibraryContext>>;
        using DllLibrary        = boost::dll::shared_library;

    private:
        IMPLEMENT_INTERFACE_CTOR(CSharpScriptEngine);

    public:
        auto GetGarbageCollector() -> Scripting::IGarbageCollector* override;

        auto CreateLibraryContext(const String& name) -> Scripting::ILibraryContext* override;
        auto CreateLibrary(const String& contextName, const String& path) -> Scripting::ILibrary* override;

    private:
        void LoadHostFxrLibrary();
        void LoadHostFxrRuntime();

    private:
        [[nodiscard]] Scripting::CSLibraryContext* GetOrCreateLibraryContext(const String& name);

    private:
        DllLibrary                  m_HostFxrLibrary;
        Scripting::CoreCLRFunctions m_CoreClr;

        Ptr<Scripting::CSGarbageCollector> m_Gc;
        LibraryContextMap                  m_LibraryContexts;
    };
} // namespace Ame::Interfaces