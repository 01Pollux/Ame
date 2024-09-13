#pragma once

#include <Core/Interface.hpp>
#include <Scripting/Types/NativeString.hpp>

namespace Ame::Scripting
{
    class ILibrary;
    class NativeString;

    class ILibraryContext : public IObject
    {
    public:
        virtual ILibrary* LoadLibrary(const NativeString& path)                                         = 0;
        virtual ILibrary* LoadLibrary(const NativeString& name, const std::byte* data, size_t dataSize) = 0;
        virtual ILibrary* GetLibrary(const NativeString& path)                                          = 0;
    };
} // namespace Ame::Scripting
