#pragma once

#include <Core/Interface.hpp>

namespace Ame::Scripting
{
    class NativeString;
    class IGarbageCollector;
    class ILibraryContext;
    class ILibrary;
    class Handle;
} // namespace Ame::Scripting

namespace Ame::Interfaces
{

    class IScriptEngine : public IObjectWithCallback
    {
    public:
        using IObjectWithCallback::IObjectWithCallback;

        [[nodiscard]] virtual auto GetGarbageCollector() -> Scripting::IGarbageCollector* = 0;

        [[nodiscard]] virtual auto CreateLibraryContext(const Scripting::NativeString& name)
            -> Scripting::ILibraryContext*                                                                    = 0;
        virtual void               TryUnloadLibraryContext(const Scripting::NativeString& name)               = 0;
        [[nodiscard]] virtual auto CreateLibrary(const Scripting::NativeString& contextName,
                                                 const Scripting::NativeString& path) -> Scripting::ILibrary* = 0;
    };
} // namespace Ame::Interfaces