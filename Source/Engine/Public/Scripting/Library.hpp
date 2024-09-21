#pragma once

#include <Core/Interface.hpp>
#include <Core/Coroutine.hpp>
#include <Scripting/Types/NativeString.hpp>
#include <Scripting/Types/NativeArray.hpp>

namespace Ame::Scripting
{
    class ILibraryContext;
    class IType;

    class ILibrary : public IObjectWithCallback
    {
    public:
        using IObjectWithCallback::IObjectWithCallback;

        [[nodiscard]] virtual NativeString GetName() const = 0;

        [[nodiscard]] virtual auto GetContext() const -> ILibraryContext*          = 0;
        [[nodiscard]] virtual auto GetType(const NativeString& name) -> Ptr<IType> = 0;
        [[nodiscard]] virtual auto GetTypes() -> Co::generator<Ptr<IType>>         = 0;
    };
} // namespace Ame::Scripting
