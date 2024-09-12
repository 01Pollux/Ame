#pragma once

#include <Core/Interface.hpp>
#include <Core/Coroutine.hpp>

namespace Ame::Scripting
{
    class ILibraryContext;
    class IType;

    class ILibrary : public IObject
    {
    public:
        [[nodiscard]] virtual auto GetContext() const -> ILibraryContext*                       = 0;
        [[nodiscard]] virtual void RegisterFunction(const String& functionName, void* callback) = 0;

        [[nodiscard]] virtual auto GetType(const String& name) -> IType* = 0;
        [[nodiscard]] virtual auto GetTypes() -> Co::generator<IType*>   = 0;
    };
} // namespace Ame::Scripting
