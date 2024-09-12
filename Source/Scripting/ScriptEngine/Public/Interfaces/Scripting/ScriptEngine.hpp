#pragma once

#include <Core/Interface.hpp>

namespace Ame::Scripting
{
    class IGarbageCollector;
    class ILibraryContext;
    class ILibrary;
    class Handle;
} // namespace Ame::Scripting

namespace Ame::Interfaces
{
    class IScriptEngine : public IObject
    {
    public:
        virtual auto GetGarbageCollector() -> Scripting::IGarbageCollector* = 0;

        virtual auto CreateLibraryContext(const String& name) -> Scripting::ILibraryContext*              = 0;
        virtual auto CreateLibrary(const String& contextName, const String& path) -> Scripting::ILibrary* = 0;
    };
} // namespace Ame::Interfaces