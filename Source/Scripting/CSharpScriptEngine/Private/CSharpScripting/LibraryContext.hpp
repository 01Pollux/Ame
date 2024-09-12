#pragma once

#include <Scripting/LibraryContext.hpp>

namespace Ame::Scripting
{
    // {3C7BD943-404B-48DC-B54B-A70D51912A5B}
    inline constexpr UId IID_CSLibraryContext{
        0x3c7bd943, 0x404b, 0x48dc, { 0xb5, 0x4b, 0xa7, 0xd, 0x51, 0x91, 0x2a, 0x5b }
    };

    class CSLibraryContext : public BaseObject<ILibraryContext>
    {
    public:
        using Base = BaseObject<ILibraryContext>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CSLibraryContext, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(CSLibraryContext) : Base(counters)
        {
        }

    public:
        ILibrary* LoadLibrary(const String& path) override;
        ILibrary* LoadLibrary(const String& path, const std::byte* data, size_t dataSize) override;
        ILibrary* GetLibrary(const String& path) override;

    private:
    };
} // namespace Ame::Scripting
