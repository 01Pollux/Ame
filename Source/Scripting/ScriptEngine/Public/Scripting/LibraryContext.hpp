#pragma once

#include <Core/Interface.hpp>

namespace Ame::Scripting
{
    class ILibrary;

    class ILibraryContext
    {
    public:
        virtual ILibrary* LoadLibrary(const String& path)                                         = 0;
        virtual ILibrary* LoadLibrary(const String& path, const std::byte* data, size_t dataSize) = 0;
        virtual ILibrary* GetLibrary(const String& path)                                          = 0;
    };
} // namespace Ame::Scripting
