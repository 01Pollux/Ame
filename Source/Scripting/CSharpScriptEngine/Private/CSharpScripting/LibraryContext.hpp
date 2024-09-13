#pragma once

#include <map>
#include <Scripting/LibraryContext.hpp>
#include <CSharpScripting/Runtime.hpp>

namespace Ame::Scripting
{
    // {3C7BD943-404B-48DC-B54B-A70D51912A5B}
    inline constexpr UId IID_CSLibraryContext{
        0x3c7bd943, 0x404b, 0x48dc, { 0xb5, 0x4b, 0xa7, 0xd, 0x51, 0x91, 0x2a, 0x5b }
    };

    class CSLibraryContext : public BaseObject<ILibraryContext>
    {
        using CreateFn = void* (*)(const NativeString& name);
        using UnloadFn = void (*)(void* context);

        using LoadLibraryFn           = void* (*)(void* context, const NativeString& path);
        using LoadLibraryFromStreamFn = void* (*)(void* context, const void* data, size_t dataSize);

        using LibraryMap = std::map<size_t, Ptr<ILibrary>>;

    public:
        using Base = BaseObject<ILibraryContext>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_CSLibraryContext, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(CSLibraryContext, const CLRRuntime& runtime, const NativeString& name);

    public:
        ~CSLibraryContext() override;

    public:
        ILibrary* LoadLibrary(const NativeString& path) override;
        ILibrary* LoadLibrary(const NativeString& name, const std::byte* data, size_t dataSize) override;
        ILibrary* GetLibrary(const NativeString& name) override;

    public:
        [[nodiscard]] auto GetHandle() const -> void*
        {
            return m_Context;
        }

    private:
        const CLRRuntime* m_Runtime = nullptr;
        void*             m_Context = nullptr;
        LibraryMap        m_Libraries;
    };
} // namespace Ame::Scripting
