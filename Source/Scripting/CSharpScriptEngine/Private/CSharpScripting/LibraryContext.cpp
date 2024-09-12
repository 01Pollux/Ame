#include <CSharpScripting/LibraryContext.hpp>

namespace Ame::Scripting
{
    ILibrary* CSLibraryContext::LoadLibrary(const String& path)
    {
        (void)path;
        return nullptr;
    }

    ILibrary* CSLibraryContext::LoadLibrary(const String& path, const std::byte* data, size_t dataSize)
    {
        (void)path;
        (void)data;
        (void)dataSize;
        return nullptr;
    }

    ILibrary* CSLibraryContext::GetLibrary(const String& path)
    {
        (void)path;
        return nullptr;
    }
} // namespace Ame::Scripting
