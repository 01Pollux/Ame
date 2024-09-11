#pragma once

#include <Core/Interface.hpp>

namespace Ame::Scripting
{
    class ILibrary;

    class ILibraryContext
    {
    public:
        virtual Ptr<ILibrary> LoadLibrary(const String& path)                                         = 0;
        virtual Ptr<ILibrary> LoadLibrary(const String& path, const std::byte* data, size_t dataSize) = 0;
        virtual Ptr<ILibrary> GetLibrary(const String& path)                                          = 0;
    };
} // namespace Ame::Scripting
