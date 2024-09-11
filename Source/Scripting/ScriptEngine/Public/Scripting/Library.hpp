#pragma once

#include <Core/Interface.hpp>
#include <Core/Coroutine.hpp>

namespace Ame::Scripting
{
    class ILibraryContext;
    class IType;

    class ILibrary
    {
    public:
        virtual auto GetContext() const -> ILibraryContext*                       = 0;
        virtual void RegisterFunction(const String& functionName, void* callback) = 0;

        virtual auto GetType(const String& name) -> Ptr<IType> = 0;
        virtual auto GetTypes() -> Co::generator<Ptr<IType>>   = 0;
    };
} // namespace Ame::Scripting
