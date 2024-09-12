#pragma once

#include <Scripting/LibraryContext.hpp>

namespace Ame::Scripting
{
    class ILibrary;

    class CSLibraryContext : public ILibraryContext
    {
    public:
        ILibrary* LoadLibrary(const String& path) override;
        ILibrary* LoadLibrary(const String& path, const std::byte* data, size_t dataSize) override;
        ILibrary* GetLibrary(const String& path) override;

    private:
    };
} // namespace Ame::Scripting
